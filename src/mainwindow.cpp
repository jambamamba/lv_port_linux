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

LeleTabView::LeleTabView(const std::string &title, const std::vector<std::string> &tab_titles){
    constexpr int32_t tab_h = 72;
    _tab_view = lv_tabview_create(lv_screen_active());
    lv_tabview_set_tab_bar_size(_tab_view, tab_h);
    lv_obj_add_event_cb(_tab_view, TabViewDeleteEventCb, LV_EVENT_DELETE, this);

    const lv_font_t *font_large = &lv_font_montserrat_24;
    const lv_font_t *font_normal = &lv_font_montserrat_16;
    lv_obj_set_style_text_font(lv_screen_active(), font_normal, 0);

    for(const std::string &tab_title: tab_titles) {
      _tabs.emplace_back(lv_tabview_add_tab(_tab_view, tab_title.c_str()));
    }

    lv_obj_t *tab_bar = lv_tabview_get_tab_bar(_tab_view);
    lv_obj_set_style_pad_left(tab_bar, LV_HOR_RES / 2, 0);
    lv_obj_t *logo = lv_image_create(tab_bar);
    lv_obj_add_flag(logo, LV_OBJ_FLAG_IGNORE_LAYOUT);
    LV_IMAGE_DECLARE(img_lvgl_logo);
    lv_image_set_src(logo, &img_lvgl_logo);
    lv_obj_align(logo, LV_ALIGN_LEFT_MID, -LV_HOR_RES / 2 + 25, 0);

    printf("@@@ data_size:%i,%i,%i,%i\n", 
      img_lvgl_logo.data_size,
      img_lvgl_logo.header.w,
      img_lvgl_logo.header.stride,
      img_lvgl_logo.header.h);

    lv_style_init(&_style_title);
    lv_style_set_text_font(&_style_title, font_large);
    lv_obj_t * label = lv_label_create(tab_bar);
    lv_obj_add_style(label, &_style_title, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_label_set_text_fmt(label, "LVGL v%d.%d.%d", lv_version_major(), lv_version_minor(), lv_version_patch());
    lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);

    label = lv_label_create(tab_bar);
    lv_label_set_text_static(label, title.c_str());
    lv_obj_add_flag(label, LV_OBJ_FLAG_IGNORE_LAYOUT);

    lv_style_init(&_style_text_muted);
    lv_style_set_text_opa(&_style_text_muted, LV_OPA_50);

    lv_obj_add_style(label, &_style_text_muted, 0);
    lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_BOTTOM, 10, 0);
}

void LeleTabView::TabViewDeleteEventCb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    LeleTabView *pthis = (LeleTabView*)e->user_data;

    if(code == LV_EVENT_DELETE) {
        lv_style_reset(&pthis->_style_text_muted);
        lv_style_reset(&pthis->_style_title);
    }
}

LeleLabel::LeleLabel(const char *text, lv_obj_t *parent, int width, int height, int corner_radius) {
    lv_style_init(&_style);
    lv_style_set_radius(&_style, corner_radius);
    lv_style_set_width(&_style, 500);
    lv_style_set_height(&_style, LV_SIZE_CONTENT);
    lv_style_set_pad_ver(&_style, 20);
    lv_style_set_pad_left(&_style, 5);
    lv_style_set_x(&_style, lv_pct(10));
    lv_style_set_y(&_style, 70);

    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_add_style(obj, &_style, 0);

    _text_box = lv_label_create(obj);
    lv_label_set_text(_text_box, text);
}

LeleTextBox::LeleTextBox(const std::string &text, lv_obj_t *parent) {
    _text_area = lv_textarea_create(parent);
    lv_textarea_set_text(_text_area, text.c_str());
    lv_obj_align(_text_area, LV_ALIGN_TOP_MID, 0, 78);
    lv_obj_set_size(_text_area, 330, 42);
    lv_textarea_set_max_length(_text_area, 15);
    lv_textarea_set_text_selection(_text_area, true);
    lv_textarea_set_one_line(_text_area, true);
    lv_obj_add_event_cb(_text_area, TextAreaEventCallback, LV_EVENT_CLICKED, _text_area);//also triggered when Enter key is pressed
}

void LeleTextBox::TextAreaEventCallback(lv_event_t * e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * _ta = (lv_obj_t *)e->user_data;

    if(code == LV_EVENT_CLICKED 
      && lv_event_get_target(e)==_ta){
        const char * text = lv_textarea_get_text(_ta);
    }
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