/*******************************************************************
 *
 * main.c - LVGL simulator for GNU/Linux
 *
 * Based on the original file from the repository
 *
 * @note eventually this file won't contain a main function and will
 * become a library supporting all major operating systems
 *
 * To see how each driver is initialized check the
 * 'src/lib/display_backends' directory
 *
 * - Clean up
 * - Support for multiple backends at once
 *   2025 EDGEMTech Ltd.
 *
 * Author: EDGEMTech Ltd, Erik Tagirov (erik.tagirov@edgemtech.ch)
 *
 ******************************************************************/
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <filesystem>
#include <optional>

#include <debug_logger/debug_logger.h>
#include <json_utils/json_utils.h>

#include <lvgl/lvgl.h>
#include <lvgl/demos/lv_demos.h>
#include <res/img_dsc.h>

#include "src/python_wrapper.h"
#include "src/graphics_backend.h"
#include "src/lelewidgets/lelebase.h"


LOG_CATEGORY(LVSIM, "LVSIM");

int main(int argc, char **argv) {
    LOG_INIT("/tmp");

    // LOG(DEBUG, LVSIM, "main %s\n", argv[0]);
    std::string input_file = (argc > 1 && *argv[1] && std::filesystem::exists(argv[1])) ? 
            argv[1] : std::filesystem::current_path().string() + "/main.py";
    if(std::filesystem::path(input_file).extension() == ".json") {
        GraphicsBackend backend;
        backend.load();
        auto tokens = LeleWidgetFactory::fromConfig(input_file);
        while(backend.handleEvents()){}
    }
    else if(std::filesystem::path(input_file).extension() == ".py") {
        if(!PythonWrapper::load(input_file)){
            LOG(FATAL, LVSIM, "Failed to load Python module\n");
        }
    }
    else {
        LOG(FATAL, LVSIM, "Missing python script or config json\n");
        return -1;
    }
    return 0;
}