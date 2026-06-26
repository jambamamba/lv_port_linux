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

#include "src/graphics_backend.h"
#include "src/lelewidgets/leleobject.h"
#include "src/python/python_wrapper.h"

#ifdef HAVE_ATSPI
#include "src/atspi/atspi_bridge.h"
#endif


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
        // Resolve to absolute path and chdir to JSON's directory so relative paths resolve
        auto json_path = std::filesystem::absolute(input_file);
        auto json_dir = json_path.parent_path();
        if(!json_dir.empty()) {
            std::filesystem::current_path(json_dir);
        }
        static LeleObject _root(nullptr);
        auto nodes = LeleWidgetFactory::fromConfig(&_root, json_path.string());

#ifdef HAVE_ATSPI
        AtspiBridge::init();
#endif

        // Let LVGL process events so the display can render
        for(int i = 0; i < 50; i++) {
            if(!GraphicsBackend::getInstance().handleEvents()) break;
        }
        // Force a full display refresh so the snapshot captures rendered content
        lv_display_t *disp = lv_disp_get_default();
        if(disp) lv_refr_now(disp);
        GraphicsBackend::getInstance().dumpScreenshot();
        LOG(DEBUG, LVSIM, "Screenshot saved to /tmp/screenshot-0.png\n");
        while(GraphicsBackend::getInstance().handleEvents()){}
    }
    else if(std::filesystem::path(input_file).extension() == ".py") {
        // Resolve to absolute path and chdir to script's directory so relative paths resolve
        auto py_path = std::filesystem::absolute(input_file);
        auto py_dir = py_path.parent_path();
        if(!py_dir.empty()) {
            std::filesystem::current_path(py_dir);
        }
        if(!PythonWrapper::load(py_path.string())) {
            LOG(WARNING, LVSIM, "Failed to run Python module: '%s'\n", py_path.c_str());
            return -1;
        }

#ifdef HAVE_ATSPI
        AtspiBridge::init();
#endif
    }
    else {
        LOG(FATAL, LVSIM, "Missing python script or config json\n");
        return -1;
    }
    return 0;
}