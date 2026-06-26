#include "graphics_backend.h"

#include <debug_logger/debug_logger.h>
#include <lvgl/lvgl.h>
#include <unistd.h>
#include <stdlib.h>
#include <mutex>

#include "src/lib/driver_backends.h"
#include "src/lib/simulator_util.h"
#include "src/lib/simulator_settings.h"

#include <src/misc/lv_types.h>
#include <src/misc/lv_event_private.h>
#include <utils/img_helper.h>

LOG_CATEGORY(LVSIM, "LVSIM");

namespace {

#if LV_USE_WAYLAND
struct window *getWindow(backend_t *backend){
    backend_type_t type = backend->type;
    if(type != BACKEND_DISPLAY) {
        return nullptr;
    }
    lv_display_t *display = backend->handle->display->display;
    // struct window *window = (struct window *)lv_display_get_user_data(display);
    struct window * window = (struct window *)lv_display_get_driver_data(display);
    if(!window) {
        return nullptr;
    }
#if 0 // Leaving it here for reference:
    lv_indev_t * lv_indev_pointer = window->lv_indev_pointer;
    lv_indev_t * lv_indev_pointeraxis = window->lv_indev_pointeraxis;
    lv_indev_t * lv_indev_touch = window->lv_indev_touch;
    lv_indev_t * lv_indev_keyboard = window->lv_indev_keyboard;
    int width = window->width;
    int height = window->height;
    bool resize_pending = window->resize_pending;
    int resize_width = window->resize_width;
    int resize_height = window->resize_height;
    bool flush_pending = window->flush_pending;
    bool shall_close = window->shall_close;
    bool closed = window->closed;
    bool maximized = window->maximized;
    bool fullscreen = window->fullscreen;
    if( window->lv_draw_buf) { //if( window->wl_ctx->shm_ctx.lv_draw_buf) {
        uint8_t * data = window->lv_draw_buf->data;
        uint32_t data_size = window->lv_draw_buf->data_size;
        int xx = 0;
        xx = 1;
    }
#endif//0
    return window;
}
#endif

auto lvColorFormatToImgHelperColorFormat(int lv_color_format) {
    switch(lv_color_format) {
        case LV_COLOR_FORMAT_RGB888: return ImgHelper::Img::ColorFormatE::RGB888;
        case LV_COLOR_FORMAT_ARGB8888: return ImgHelper::Img::ColorFormatE::RGBA8888;
        default: LL(FATAL, LVSIM) << "Invalid bytes per pixel (bpp):" << lv_color_format; return ImgHelper::Img::ColorFormatE::RGBA8888;
    }
}

}//namespace

GraphicsBackend &GraphicsBackend::getInstance() {
    static GraphicsBackend backend;
    return backend;
}
lv_point_t GraphicsBackend::getTouchPoint(lv_obj_t *obj) const {
#if LV_USE_WAYLAND
    lv_point_t point = {};
    struct window *window = getWindow(_backend);
    lv_indev_get_point(window->lv_indev_pointer, &point);

    if(obj) {
        lv_area_t obj_area;
        lv_obj_get_coords(obj, &obj_area);
        lv_point_t obj_pt = {0};
        obj_pt.x = point.x - obj_area.x1;
        obj_pt.y = point.y - obj_area.y1;
        return obj_pt;
    }

    return point;
#else
    return lv_point_t{};
#endif
}

backend_t *GraphicsBackend::getBackend() const {
    return _backend;
}
void GraphicsBackend::dumpScreenshot() const {

    lv_obj_t *screen = lv_scr_act();
    LOG(DEBUG, LVSIM, "dumpScreenshot: screen=%p w=%d h=%d\n", (void*)screen,
        screen ? lv_obj_get_width(screen) : -1,
        screen ? lv_obj_get_height(screen) : -1);
    if(!screen) {
        LOG(WARNING, LVSIM, "Screenshot failed: no active screen\n");
        return;
    }
    int n_child = lv_obj_get_child_count(screen);
    LOG(DEBUG, LVSIM, "dumpScreenshot: screen children=%d\n", n_child);
    for(int ci = 0; ci < n_child && ci < 10; ci++) {
        lv_obj_t *child = lv_obj_get_child(screen, ci);
        if(child) {
            LOG(DEBUG, LVSIM, "  child[%d]=%p w=%d h=%d x=%d y=%d\n", ci, (void*)child,
                lv_obj_get_width(child), lv_obj_get_height(child),
                lv_obj_get_x(child), lv_obj_get_y(child));
        }
    }

    lv_draw_buf_t *snapshot = lv_snapshot_take(screen, LV_COLOR_FORMAT_ARGB8888);
    if (!snapshot) {
        LOG(WARNING, LVSIM, "Screenshot failed: lv_snapshot_take returned null\n");
        return;
    }

    int w = snapshot->header.w;
    int h = snapshot->header.h;
    int stride = snapshot->header.stride;
    uint32_t cf = snapshot->header.cf;
    uint8_t *data = static_cast<uint8_t*>(snapshot->data);
    LOG(DEBUG, LVSIM, "Snapshot: %dx%d stride=%d cf=%d\n", w, h, stride, (int)cf);

    // LVGL stores ARGB8888 as [B, G, R, A] on little-endian; ImageMagick RGBA expects [R, G, B, A]
    // Swap B and R bytes since there's no stride padding (stride == w*4)
    for(int y = 0; y < h; y++) {
        uint8_t *row = data + y * stride;
        for(int x = 0; x < w; x++) {
            uint8_t *px = row + x * 4;
            uint8_t tmp = px[0];
            px[0] = px[2];
            px[2] = tmp;
        }
    }



    static int i = 0;
    std::stringstream ss;
    ss << "/tmp/screenshot-" << std::to_string(i) << ".png";
    int bpp = 4;
    ImgHelper::saveToFile(
            ss.str(),
            w, h,
            stride,
            bpp,
            lvColorFormatToImgHelperColorFormat((int)cf),
            data);
    lv_draw_buf_destroy(snapshot);
    i = (i+1)%100;
}

// Global simulator settings, defined in lv_linux_backend.c
extern simulator_settings_t settings;
bool GraphicsBackend::load(int screen_width, int screen_height) {
    LOG(DEBUG, LVSIM, "GraphicsBackend::load(%d, %d)\n", screen_width, screen_height);
    driver_backends_register();//connect to wayland server
    settings.window_width = screen_width;
    settings.window_height = screen_height;
    // settings.fullscreen = true;
    // settings.maximize = true;
    lv_init();
    char selected_backend[] = "WAYLAND";
    LOG(DEBUG, LVSIM, "GraphicsBackend::load calling driver_backends_init_backend\n");
    _backend = driver_backends_init_backend(selected_backend);
    if(!_backend) {
        LOG(WARNING, LVSIM, "Failed to initialize display backend\n");
        return false;
    }
    return true;
}

bool GraphicsBackend::handleEvents() const {
    fflush(stdout);
#if LV_USE_WAYLAND
    bool timer_ret = lv_wayland_timer_handler();
    bool window_open = lv_wayland_window_is_open(nullptr);
    // LOG(DEBUG, LVSIM, "handleEvents: timer_ret=%d window_open=%d\n", timer_ret, window_open);
    if (!window_open) {
        LOG(DEBUG, LVSIM, "Exiting event loop because all windows are closed\n");
        fflush(stdout);
        return false;
    }
#else
    lv_timer_handler();
#endif
    return true;
}
