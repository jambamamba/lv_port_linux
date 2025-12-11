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

#include "src/python/python_wrapper.h"
#include "src/graphics_backend.h"
#include "src/lelewidgets/leleobject.h"


LOG_CATEGORY(LVSIM, "LVSIM");

int main(int argc, char **argv) {
    LOG_INIT("/tmp");

    LOG(DEBUG, LVSIM, "main %s\n", argv[1]);
    if(argc < 2 || !*argv[1]) {
        LOG(FATAL, LVSIM, "Missing argument, either path to config json, or path to a python file\n");
        return -1;
    }
    std::string input_file = argv[1];
    if(std::filesystem::path(input_file).extension() == ".json") {
        GraphicsBackend backend;
        backend.load();
        auto tokens = LeleWidgetFactory::fromConfig(input_file);
        while(backend.handleEvents()){}
    }
    else if(std::filesystem::path(input_file).extension() == ".py") {
        if(!PythonWrapper::load(input_file)) {
            LOG(FATAL, LVSIM, "Failed to run Python module: '%s'\n", input_file.c_str());
        }
    }
    else {
        LOG(FATAL, LVSIM, "Missing python script or config json\n");
        return -1;
    }
    return 0;
}