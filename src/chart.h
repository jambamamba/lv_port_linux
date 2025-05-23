#pragma once

#include <lvgl/lvgl.h>
#include <lvgl/lvgl_private.h>

#include <string>
#include <vector>

lv_obj_t *
createChart(
    lv_obj_t *parent,
    const char *id,
    int posx,
    int posy,
    int width,
    int height,
    int ymin,
    int ymax,
    std::vector<std::string> &xlabels,
    std::vector<std::string> &ylabels,
    std::vector<std::string> &series_names,
    std::string update_timer_function,
    const std::string &type = "line",
    int update_hz = 10
    );
