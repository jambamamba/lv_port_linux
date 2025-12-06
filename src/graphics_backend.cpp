#include "graphics_backend.h"

#include <debug_logger/debug_logger.h>
#include <lvgl/lvgl.h>
#include <unistd.h>
#include <stdlib.h>

#include "src/lib/driver_backends.h"
#include "src/lib/simulator_util.h"
#include "src/lib/simulator_settings.h"

LOG_CATEGORY(LVSIM, "LVSIM");

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
    if (driver_backends_init_backend(selected_backend) == -1) {
        LOG(FATAL, LVSIM, "Failed to initialize display backend\n");
        return false;
    }
    return true;
}

bool GraphicsBackend::handleEvents() {
    // LOG(DEBUG, LVSIM, "@@@@@ handleEvents\n");
    if (lv_wayland_timer_handler()) {
        // Wait only if the cycle was completed
        // usleep(LV_DEF_REFR_PERIOD * 1000);//osm: adjust this LV_DEF_REFR_PERIOD for device
    }
    // Run until the last window closes
    if (!lv_wayland_window_is_open(NULL)) {
        LOG(DEBUG, LVSIM, "Exiting event loop because all windows are closed\n");
        return false;
    }
    return true;
}
