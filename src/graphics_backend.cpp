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
    settings.fullscreen = false;
    settings.maximize = false;
    lv_init();
    char selected_backend[] = "WAYLAND";
    _backend = driver_backends_init_backend(selected_backend);
    // if (driver_backends_init_backend(selected_backend) == -1) {
    if(!_backend) {
        LOG(FATAL, LVSIM, "Failed to initialize display backend\n");
        return false;
    }
    // struct window *window = getWindow(_backend);
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
    lv_indev_get_point(getWindow(_backend)->lv_indev_pointer, &point);
    // LOG(DEBUG, LVSIM, "@@@@@ point:%i,%i\n", point.x, point.y);
    return true;
}
