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

//does not work:
// void keyboard_cb(lv_event_t * e) {
//     switch(e->code){
//         case LV_EVENT_KEY:{
//             struct window *window = (struct window *) e->user_data;
//             uint32_t key = lv_indev_get_key(window->lv_indev_keyboard);
//             LOG(DEBUG, LVSIM, "@@@@@ key:%i\n", key);
//             break;
//         }
//         case LV_EVENT_CLICKED:{
//             struct window *window = (struct window *) e->user_data;
//             // uint32_t key = lv_indev_get_key(window->lv_indev_keyboard);
//             // LOG(DEBUG, LVSIM, "@@@@@ key:%i\n", key);
//             break;
//         }
//         default:
//             LOG(DEBUG, LVSIM, "@@@@@ e->code:%i\n", e->code);
//             break;
//     }
// }
}//namespace
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

bool GraphicsBackend::handleEvents() {
    // LOG(DEBUG, LVSIM, "@@@@@ handleEvents\n");
    if (lv_wayland_timer_handler()) {
        // Wait only if the cycle was completed
        // usleep(LV_DEF_REFR_PERIOD * 1000);//osm: adjust this LV_DEF_REFR_PERIOD for device
    }
    // Run until the last window closes
    if (!lv_wayland_window_is_open(nullptr)) {
        LOG(DEBUG, LVSIM, "Exiting event loop because all windows are closed\n");
        return false;
    }
    lv_point_t point = {};
    struct window *window = getWindow(_backend);
    lv_indev_get_point(window->lv_indev_pointer, &point);
    // LOG(DEBUG, LVSIM, "@@@@@ point:%i,%i\n", point.x, point.y);

    // if(window->wl_ctx->shm_ctx.lv_draw_buf) {
    //     uint8_t *data = window->wl_ctx->shm_ctx.lv_draw_buf->data;
    //     uint32_t data_size = window->wl_ctx->shm_ctx.lv_draw_buf->data_size;
    //     lv_image_header_t &header = window->wl_ctx->shm_ctx.lv_draw_buf->header;
    //     int xx = 0;
    //     xx = 1;

    //     if(data_size!=192000){
    //         return true;
    //     }
    //     static int i = 0;
    //     std::stringstream ss;
    //     ss << "/home/oosman/Downloads/foo/foo" << std::to_string(i++) << ".png";
    //     ImgHelper::saveGdImage(
    //             ss.str().c_str(),
    //             header.w, 
    //             header.h, 
    //             header.stride, 
    //             header.stride/header.w, 
    //             data);
    // }
    return true;
}
