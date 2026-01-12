#include "graphics_backend.h"

#include <debug_logger/debug_logger.h>
#include <lvgl/lvgl.h>
#include <unistd.h>
#include <stdlib.h>

#include "src/lib/driver_backends.h"
#include "src/lib/simulator_util.h"
#include "src/lib/simulator_settings.h"

#include <src/misc/lv_types.h>
#include <src/misc/lv_event_private.h>
#include <utils/img_helper.h>

LOG_CATEGORY(LVSIM, "LVSIM");

namespace {
struct window *getWindow(backend_t *backend){
    backend_type_t type = backend->type;
    if(type == BACKEND_DISPLAY) {
        lv_display_t *display = backend->handle->display->display;
        struct window * window = (struct window *)lv_display_get_user_data(display);
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
        if( window->wl_ctx->shm_ctx.lv_draw_buf) {
            uint8_t * data = window->wl_ctx->shm_ctx.lv_draw_buf->data;
            uint32_t data_size = window->wl_ctx->shm_ctx.lv_draw_buf->data_size;
            int xx = 0;
            xx = 1;
        }
        return window;
    }
    return nullptr;
}
}//namespace

GraphicsBackend &GraphicsBackend::getInstance() {
    static GraphicsBackend backend;
    return backend;
}
lv_point_t GraphicsBackend::getTouchPoint(lv_obj_t *obj) const {

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
}

backend_t *GraphicsBackend::getBackend() const {
    return _backend;
}
void GraphicsBackend::dumpScreenshot() const {

    lv_obj_t *screen = lv_scr_act();
    lv_area_t snapshot_area = {0};
    lv_draw_buf_t * snapshot = lv_snapshot_take(screen, LV_COLOR_FORMAT_RGB888);

    static int i = 0;
    std::stringstream ss;
    ss << "/home/oosman/Downloads/foo/foo" << std::to_string(i) << ".png";
    ImgHelper::saveGdImage(
            ss.str().c_str(),
            snapshot->header.w, 
            snapshot->header.h, 
            snapshot->header.stride, 
            snapshot->header.stride/snapshot->header.w, 
            snapshot->data);
    lv_draw_buf_destroy(snapshot);
    i = (i+1)%100;
}

// Global simulator settings, defined in lv_linux_backend.c
extern simulator_settings_t settings;
bool GraphicsBackend::load() {
    driver_backends_register();//connect to wayland server
    settings.window_width = atoi(getenv("LV_SIM_WINDOW_WIDTH") ? : "800");//osm todo get these values from config
    settings.window_height = atoi(getenv("LV_SIM_WINDOW_HEIGHT") ? : "480");
    // settings.fullscreen = true;
    // settings.maximize = true;
    lv_init();
    char selected_backend[] = "WAYLAND";
    _backend = driver_backends_init_backend(selected_backend);
    // if (driver_backends_init_backend(selected_backend) == -1) {
    if(!_backend) {
        LOG(FATAL, LVSIM, "Failed to initialize display backend\n");
        return false;
    }
    struct window *window = getWindow(_backend);
    // lv_indev_add_event_cb(window->lv_indev_keyboard, keyboard_cb, LV_EVENT_KEY, window);
    // lv_indev_add_event_cb(window->lv_indev_pointer, keyboard_cb, LV_EVENT_CLICKED, window);
    return true;
}

bool GraphicsBackend::handleEvents() const {
    // LL(DEBUG, LVSIM) << "@@@@@ handleEvents";
    // static auto start = std::chrono::high_resolution_clock::now();
    // auto end = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    // if(duration.count() > 100) {
    //     LL(DEBUG, LVSIM) << "@@@@@ handleEvents, " << duration.count() << " us since last call";
    // }
    // start = end;

    if (lv_wayland_timer_handler()) {
        // Wait only if the cycle was completed
        // usleep(LV_DEF_REFR_PERIOD * 1000);//osm: adjust this LV_DEF_REFR_PERIOD for device
    }
    // Run until the last window closes
    if (!lv_wayland_window_is_open(nullptr)) {
        LOG(DEBUG, LVSIM, "Exiting event loop because all windows are closed\n");
        return false;
    }
    // lv_point_t point = {};
    // struct window *window = getWindow(_backend);
    // lv_indev_get_point(window->lv_indev_pointer, &point);
    // dumpScreenshot();
    return true;
}
