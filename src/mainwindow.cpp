#include <lvgl/lvgl_private.h>
#include <string>
#include <vector>

#include "mainwindow.h"
#include "chart.h"

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN && LV_MEM_SIZE < (38ul * 1024ul)
    #error Insufficient memory for lv_demo_widgets. Please set LV_MEM_SIZE to at least 38KB (38ul * 1024ul).  48KB is recommended.
#endif

typedef enum {
    DISP_SMALL,
    DISP_MEDIUM,
    DISP_LARGE,
} disp_size_t;

void 
addTextBox()
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_radius(&style, 5);

    lv_style_set_width(&style, 500);
    lv_style_set_height(&style, LV_SIZE_CONTENT);

    lv_style_set_pad_ver(&style, 20);
    lv_style_set_pad_left(&style, 5);

    lv_style_set_x(&style, lv_pct(10));
    lv_style_set_y(&style, 70);

    /*Create an object with the new style*/
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_add_style(obj, &style, 0);

    static lv_obj_t * _label_serialnum;
    _label_serialnum = lv_label_create(obj);
    lv_label_set_text(_label_serialnum, "Love");
}

static void 
textAreaEventCallback(lv_event_t * e){ 
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * _ta = (lv_obj_t *)e->user_data;

    if(code == LV_EVENT_CLICKED 
      && lv_event_get_target(e)==_ta){
        const char * text = lv_textarea_get_text(_ta);
    }
}

void 
addTextArea()
{
    static lv_obj_t * _ta = lv_textarea_create(lv_screen_active());
    lv_textarea_set_text(_ta, "Hello World");
    lv_obj_align(_ta, LV_ALIGN_TOP_MID, 0, 78);
    lv_obj_set_size(_ta, 330, 42);
    lv_textarea_set_max_length(_ta, 15);
    lv_textarea_set_text_selection(_ta, true);
    lv_textarea_set_one_line(_ta, true);
    lv_obj_add_event_cb(_ta, textAreaEventCallback, LV_EVENT_CLICKED, _ta);//also triggered when Enter key is pressed
}

void 
addStatusMessage()
{
    static lv_obj_t * _label_status = lv_label_create(lv_screen_active());
    lv_label_set_text(_label_status, "");
    lv_obj_align(_label_status, LV_ALIGN_TOP_MID, 0, 148);
    lv_obj_set_size(_label_status, 330, 42);
}

void 
addLoaderArc()
{
  static lv_obj_t * _arc = lv_arc_create(lv_screen_active());
  lv_arc_set_bg_angles(_arc, 0, 360);
  lv_arc_set_angles(_arc, 270, 270);
  lv_obj_align(_arc, LV_ALIGN_CENTER, 0, 0);
  // lv_obj_set_size(_arc, 0, 0);
}

void 
addProgressBar()
{
  static lv_obj_t * _progress_bar = lv_bar_create(lv_screen_active());
  lv_obj_set_size(_progress_bar, 200, 20);
  lv_obj_align(_progress_bar, LV_ALIGN_CENTER, 0, 0);
  // lv_bar_set_anim_time(_progress_bar, 2000);
  lv_bar_set_value(_progress_bar, 0, LV_ANIM_ON);
}

void
addChart(){

  std::vector<std::string> xlabels = {"S","T","K","P","W","H","R","A","O","*","E","U","F","-R","-P","B","L","G","-T","-S","D","Z","#","!","UU"};
  std::vector<std::string> ylabels = {"","","","","","","","","",""};
  std::vector<std::string> series_names = {"series1"};

  static lv_obj_t *obj =
    createChart(
        lv_screen_active(),
        "my chart",
        40, 220, 
        500, 100,
        0, 10,
        xlabels, ylabels,
        series_names,
        "updateChart1"
      );
}