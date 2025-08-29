#include "mainwindow.h"
#include "chart.h"

#include <string.h>
#include <vector>

LOG_CATEGORY(LVSIM, "LVSIM");

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN && LV_MEM_SIZE < (38ul * 1024ul)
    #error Insufficient memory for lv_demo_widgets. Please set LV_MEM_SIZE to at least 38KB (38ul * 1024ul).  48KB is recommended.
#endif

typedef enum {
    DISP_SMALL,
    DISP_MEDIUM,
    DISP_LARGE,
} disp_size_t;


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