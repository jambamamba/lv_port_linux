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

#include "src/lib/driver_backends.h"
#include "src/lib/simulator_util.h"
#include "src/lib/simulator_settings.h"

#include "mainwindow.h"

LOG_CATEGORY(LVSIM, "LVSIM");

/* Global simulator settings, defined in lv_linux_backend.c */
extern simulator_settings_t settings;

namespace {
std::optional<std::unique_ptr<LeleTabView>> parseConfig() {
    std::string config_json(std::filesystem::current_path());
    config_json += "/config.json";
    const cJSON* root = readJson(config_json.c_str());
    if(!root) {
        LOG(DEBUG, LVSIM, "Failed to failed to load file: '%s'\n", config_json.c_str());
        return std::nullopt;
    }
    const cJSON *tabview = objFromJson(root, "tabview");
    if(!tabview) {
        LOG(DEBUG, LVSIM, "Failed to load tabview from config_json:'%s'\n", config_json.c_str());
        return std::nullopt;
    }
    return LeleTabView::fromJson(tabview);
}

/* Internal functions */
static void print_lvgl_version(void);
static void print_usage(void);

/* contains the name of the selected backend if user
 * has specified one on the command line */
static char *selected_backend;

/**
 * @brief Print LVGL version
 */
static void print_lvgl_version(void)
{
    fprintf(stdout, "%d.%d.%d-%s\n",
            LVGL_VERSION_MAJOR,
            LVGL_VERSION_MINOR,
            LVGL_VERSION_PATCH,
            LVGL_VERSION_INFO);
}

/**
 * @brief Print usage information
 */
static void print_usage(void)
{
    fprintf(stdout, "\nlvglsim [-V] [-B] [-b backend_name] [-W window_width] [-H window_height]\n\n");
    fprintf(stdout, "-V print LVGL version\n");
    fprintf(stdout, "-B list supported backends\n");
}

/**
 * @brief Configure simulator
 * @description process arguments recieved by the program to select
 * appropriate options
 * @param argc the count of arguments in argv
 * @param argv The arguments
 */
static void configureSimulator(int argc, char **argv)
{
    int opt = 0;
    char *backend_name;

    selected_backend = NULL;
    driver_backends_register();

    /* Default values */
    settings.window_width = atoi(getenv("LV_SIM_WINDOW_WIDTH") ? : "800");
    settings.window_height = atoi(getenv("LV_SIM_WINDOW_HEIGHT") ? : "480");

    /* Parse the command-line options. */
    while ((opt = getopt (argc, argv, "b:fmW:H:BVh")) != -1) {
        switch (opt) {
        case 'h':
            print_usage();
            exit(EXIT_SUCCESS);
            break;
        case 'V':
            print_lvgl_version();
            exit(EXIT_SUCCESS);
            break;
        case 'B':
            driver_backends_print_supported();
            exit(EXIT_SUCCESS);
            break;
        case 'b':
            if (driver_backends_is_supported(optarg) == 0) {
                die("error no such backend: %s\n", optarg);
            }
            selected_backend = strdup(optarg);
            break;
        case 'W':
            settings.window_width = atoi(optarg);
            break;
        case 'H':
            settings.window_height = atoi(optarg);
            break;
        case ':':
            print_usage();
            die("Option -%c requires an argument.\n", optopt);
            break;
        case '?':
            print_usage();
            die("Unknown option -%c.\n", optopt);
        }
    }
}
}//namespace

/**
 * @brief entry point
 * @description start a demo
 * @param argc the count of arguments in argv
 * @param argv The arguments
 */
int main(int argc, char **argv)
{
    LOG_INIT("/tmp");

    LOG(DEBUG, LVSIM, "configure simulator\n");
    configureSimulator(argc, argv);

    /* Initialize LVGL. */
    lv_init();

    /* Initialize the configured backend */
    if (driver_backends_init_backend(selected_backend) == -1) {
        die("Failed to initialize display backend");
    }

    /* Enable for EVDEV support */
#if LV_USE_EVDEV
    char evdev[] = "EVDEV";
    if (driver_backends_init_backend(evdev) == -1) {
        die("Failed to initialize evdev");
    }
#endif

    /*Create a Demo*/
    // lv_demo_widgets();
    // lv_demo_widgets_start_slideshow();

    // addTextArea();
    // addStatusMessage();
    // addLoaderArc();
    // addProgressBar();
    // addChart();
    LOG(DEBUG, LVSIM, "create tab view\n");

    auto tab_view = parseConfig();
    // if(tab_titles.size() > 0) {
    //     LeleTabView tab_view("tabview", "logo.png", tab_titles);
    //     LeleLabel label1("Label1", tab_view._tabs.at(0), 10, 70, 500);
    //     LeleTextBox text_box1("Textbox1", label1.obj(), 100, 0, 300);
    //     LeleLabel label2("Label2", tab_view._tabs.at(1), 10, 70, 500);
    // }
    
    /* Enter the run loop of the selected backend */
    driver_backends_run_loop();

    return 0;
}


