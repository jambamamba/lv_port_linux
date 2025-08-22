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

LeleTabView::LeleTabView(
  const std::string &title,
  const std::string &subtitle,
  const std::string &logo_img,
  const std::string &fgcolor_str,
  const std::string &bgcolor_str,
  const std::string &active_tab_bgcolor_str,
  const std::string &active_tab_bottom_border_color_str,
  const std::vector<LeleTabView::Tab> &tabs){

    int fgcolor = std::stoi(fgcolor_str, nullptr, 16);
    int bgcolor = std::stoi(bgcolor_str, nullptr, 16);
    int active_tab_color = std::stoi(active_tab_bgcolor_str, nullptr, 16);
    int active_tab_bottom_border_color = std::stoi(active_tab_bottom_border_color_str, nullptr, 16);

    constexpr int32_t tab_h = 75;
    _lv_obj = lv_tabview_create(lv_screen_active());
    lv_tabview_set_tab_bar_size(_lv_obj, tab_h);
    lv_obj_add_event_cb(_lv_obj, tabViewDeleteEventCb, LV_EVENT_DELETE, this);

    const lv_font_t *font_normal = &lv_font_montserrat_16;
    lv_obj_set_style_text_font(lv_screen_active(), font_normal, 0);
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(fgcolor), LV_PART_MAIN);
    lv_obj_set_style_bg_color(_lv_obj, lv_color_hex(bgcolor), LV_PART_MAIN);
    
    lv_obj_t *tabview_content = lv_tabview_get_content(_lv_obj);
    lv_obj_t *tabview_header = lv_tabview_get_tab_bar(_lv_obj);
    lv_obj_set_style_text_color(tabview_header, lv_color_hex(fgcolor), LV_PART_MAIN);
    lv_obj_set_style_bg_color(tabview_header, lv_color_hex(bgcolor), LV_PART_MAIN);

    for(auto &&tab_: tabs) {
        _tabs.emplace_back(tab_);
        auto &tab = _tabs[_tabs.size() - 1];
        tab.setLvObj(
          lv_tabview_add_tab(_lv_obj, tab.title().c_str()));
        lv_obj_t *button = lv_obj_get_child(tabview_header, _tabs.size() - 1);
        tab.setTabButton(button, active_tab_color, active_tab_bottom_border_color);
        tab.loadTabContent();
    }

    lv_obj_t *logo = setTabViewImg(tabview_header, logo_img);
    lv_obj_t *label = setTabViewTitle(tabview_header, title);
    lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);
    label = setTabViewSubTitle(tabview_header, subtitle);
    lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_BOTTOM, 10, 0);
}

lv_obj_t *LeleTabView::setTabViewImg(lv_obj_t *tabview_header, const std::string &logo_img) {
    lv_obj_set_style_pad_left(tabview_header, LV_HOR_RES / 2, 0);
    lv_obj_t *logo = lv_image_create(tabview_header);
    lv_obj_add_flag(logo, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_image_set_src(logo, _lv_img_dsc_map.at(logo_img));
    lv_obj_align(logo, LV_ALIGN_LEFT_MID, -LV_HOR_RES / 2 + 25, 0);
    return logo;
}

lv_obj_t *LeleTabView::setTabViewTitle(lv_obj_t *tabview_header, const std::string &title) {
    lv_obj_t *label = lv_label_create(tabview_header);
    lv_style_init(&_style_title);
    const lv_font_t *font_large = &lv_font_montserrat_24;
    lv_style_set_text_font(&_style_title, font_large);
    lv_obj_add_style(label, &_style_title, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_label_set_text_fmt(label, title.c_str());//"LVGL v%d.%d.%d", lv_version_major(), lv_version_minor(), lv_version_patch());
    return label;
}

lv_obj_t *LeleTabView::setTabViewSubTitle(lv_obj_t *tabview_header, const std::string &subtitle) {
    lv_obj_t *label = lv_label_create(tabview_header);
    lv_style_init(&_style_text_muted);
    lv_style_set_text_opa(&_style_text_muted, LV_OPA_50);
    lv_obj_add_style(label, &_style_text_muted, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_label_set_text(label, subtitle.c_str());
    return label;
}

void LeleTabView::tabViewDeleteEventCb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    LeleTabView *pthis = (LeleTabView*)e->user_data;

    if(code == LV_EVENT_DELETE) {
        lv_style_reset(&pthis->_style_text_muted);
        lv_style_reset(&pthis->_style_title);
    }
}

std::unique_ptr<LeleTabView> LeleTabView::fromJson(const cJSON *tabview) {

    const cJSON *title = objFromJson(tabview, "title");
    if(!title) {
      LOG(WARNING, LVSIM, "tabview is missing title\n");
    }
    LOG(DEBUG, LVSIM, "@@@ %s:%s\n", title->string, title->valuestring);
    std::string title_str = title->valuestring;

    const cJSON *subtitle = objFromJson(tabview, "subtitle");
    if(!subtitle) {
      LOG(WARNING, LVSIM, "tabview is missing subtitle\n");
    }
    LOG(DEBUG, LVSIM, "@@@ %s:%s\n", subtitle->string, subtitle->valuestring);
    std::string subtitle_str = subtitle->valuestring;

    const cJSON *img = objFromJson(tabview, "img");
    if(!img) {
      LOG(WARNING, LVSIM, "tabview is missing img\n");
    }
    LOG(DEBUG, LVSIM, "@@@ %s:%s\n", img->string, img->valuestring);
    std::string img_str = img->valuestring;
    
    const cJSON *fgcolor = objFromJson(tabview, "fgcolor");
    if(!fgcolor) {
      LOG(WARNING, LVSIM, "tabview is missing fgcolor\n");
    }
    LOG(DEBUG, LVSIM, "@@@ %s:%s\n", fgcolor->string, fgcolor->valuestring);
    std::string fgcolor_str = fgcolor->valuestring;

    const cJSON *bgcolor = objFromJson(tabview, "bgcolor");
    if(!bgcolor) {
      LOG(WARNING, LVSIM, "tabview is missing bgcolor\n");
    }
    LOG(DEBUG, LVSIM, "@@@ %s:%s\n", bgcolor->string, bgcolor->valuestring);
    std::string bgcolor_str = bgcolor->valuestring;

    const cJSON *active_tab_bgcolor = objFromJson(tabview, "active_tab_bgcolor");
    if(!bgcolor) {
      LOG(WARNING, LVSIM, "tabview is missing active_tab_bgcolor\n");
    }
    LOG(DEBUG, LVSIM, "@@@ %s:%s\n", active_tab_bgcolor->string, active_tab_bgcolor->valuestring);
    std::string active_tab_bgcolor_str = active_tab_bgcolor->valuestring;

    const cJSON *active_tab_bottom_border_color = objFromJson(
      tabview, "active_tab_bottom_border_color");
    if(!bgcolor) {
      LOG(WARNING, LVSIM, "tabview is missing active_tab_bottom_border_color\n");
    }
    LOG(DEBUG, LVSIM, "@@@ %s:%s\n", active_tab_bottom_border_color->string, active_tab_bottom_border_color->valuestring);
    std::string active_tab_bottom_border_color_str = active_tab_bottom_border_color->valuestring;

    const cJSON *json_tabs = objFromJson(tabview, "tabs");
    if(!json_tabs) {
        LOG(WARNING, LVSIM, "tabview is missing tabs\n");
        return std::unique_ptr<LeleTabView>();
    }
    std::vector<LeleTabView::Tab> tabs;
    if(cJSON_IsArray(json_tabs)) {
        cJSON *json_tab = nullptr;
        cJSON_ArrayForEach(json_tab, json_tabs) {
            tabs.emplace_back(
              LeleTabView::Tab::fromJson(json_tab));
        }
    }

    return std::make_unique<LeleTabView>(
      title_str, 
      subtitle_str, 
      img_str, 
      fgcolor_str, 
      bgcolor_str, 
      active_tab_bgcolor_str, 
      active_tab_bottom_border_color_str, 
      tabs);
}

LeleTabView::Tab LeleTabView::Tab::fromJson(const cJSON *json_tab) {
  std::string name; 
  std::string img;
  std::string content;
  cJSON *item = nullptr;
  cJSON_ArrayForEach(item, json_tab) {
      LOG(DEBUG, LVSIM, "@@@ %s:%s\n", item->string, item->valuestring);
      if(strcmp(item->string, "name")==0) {
          name = cJSON_GetStringValue(item);
      }
      else if(strcmp(item->string, "img")==0) {
          img = cJSON_GetStringValue(item);
      }
      else if(strcmp(item->string, "content")==0) {
          content = cJSON_Print(item);//This has bug: cJSON_Duplicate(item, true);//dont forget to cJSON_Delete
      }
  }
  return LeleTabView::Tab(name, img, content);
}

void LeleTabView::Tab::setTabButton(lv_obj_t *button, int active_tab_bgcolor, int active_tab_bottom_border_color) {
  _tab_button = button;
  lv_obj_t *logo = lv_image_create(button);
  lv_obj_add_flag(logo, LV_OBJ_FLAG_IGNORE_LAYOUT);
  lv_image_set_src(logo, _lv_img_dsc_map.at(_img.c_str()));
  lv_obj_center(logo);
  lv_obj_t *label = lv_obj_get_child(button, 0);
  lv_label_set_text(label, "");

  lv_obj_set_style_bg_color(button, lv_color_hex(active_tab_bgcolor), LV_PART_MAIN | LV_STATE_CHECKED);
  lv_obj_set_style_bg_color(button, lv_color_hex(active_tab_bgcolor), LV_PART_MAIN | LV_STATE_PRESSED);
  lv_obj_set_style_border_color(button, lv_color_hex(active_tab_bottom_border_color), LV_PART_MAIN | LV_STATE_CHECKED);
}

void LeleTabView::Tab::loadTabContent() {
  if(_content.empty()) {
    return;
  }
  cJSON *items = cJSON_Parse(_content.c_str());
  cJSON *item = nullptr;
  cJSON_ArrayForEach(item, items) {
      // LOG(DEBUG, LVSIM, "@@@ tab content: %s\n", item->string);
      if(strcmp(item->string, "label") == 0) {
        LOG(DEBUG, LVSIM, "@@@ tab content: %s\n", item->string);
        //osm todo: load LeleLabel from cJSON 'item' object, and insert it into the tab.
        //osm todo: tab should have a list of all items.
      }
      if(strcmp(item->string, "textbox") == 0) {
        LOG(DEBUG, LVSIM, "@@@ tab content: %s\n", item->string);
        //osm todo: load LeleTextBox from cJSON 'item' object, and insert it into the tab.
        //osm todo: tab should have a list of all items.
      }
  }
    
  // LOG(DEBUG, LVSIM, "num tabs: %i\n", _tabs.size());
  // labelx.emplace_back(std::make_unique<LeleLabel>(
  //     "asdfafda",
  //     lv_tab,
  //     lv_obj_get_x(tabview_content), 
  //     lv_obj_get_y(tabview_content),
  //     lv_obj_get_width(tabview_content)/2, 
  //     lv_obj_get_height(tabview_content)/4 
  // ));
  // textx.emplace_back(std::make_unique<LeleTextBox>(
  //     "asdfafda",
  //     lv_tab,
  //     lv_obj_get_x(tabview_content), 
  //     lv_obj_get_y(tabview_content)+lv_obj_get_height(tabview_content)/4, 
  //     lv_obj_get_width(tabview_content)/2, 
  //     lv_obj_get_height(tabview_content)/4 
  // ));
}

LeleLabel::LeleLabel(const char *text, lv_obj_t *parent, int x, int y, int width, int height, int corner_radius) {
    lv_style_init(&_style);
    lv_style_set_radius(&_style, corner_radius);
    lv_style_set_width(&_style, width);
    lv_style_set_height(&_style, height);
    lv_style_set_pad_ver(&_style, 20);
    lv_style_set_pad_left(&_style, 5);
    lv_style_set_x(&_style, lv_pct(x));
    lv_style_set_y(&_style, y);

    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_set_pos(obj, x, y);
    lv_obj_add_style(obj, &_style, 0);

    _text_box = lv_label_create(obj);
    lv_label_set_text(_text_box, text);
}

LeleTextBox::LeleTextBox(const std::string &text, lv_obj_t *parent, int x, int y, int width, int height, int corner_radius) {
    lv_style_init(&_style);
    lv_style_set_radius(&_style, corner_radius);
    lv_style_set_width(&_style, width);
    lv_style_set_height(&_style, height);
    lv_style_set_pad_ver(&_style, 20);
    lv_style_set_pad_left(&_style, 5);
    lv_style_set_x(&_style, lv_pct(x));
    lv_style_set_y(&_style, y);

    _text_area = lv_textarea_create(parent);
    lv_obj_add_style(_text_area, &_style, 0);
    lv_obj_set_pos(_text_area, x, y);

    lv_textarea_set_text(_text_area, text.c_str());
    // lv_obj_align(_text_area, LV_ALIGN_TOP_MID, x, y);
    // lv_obj_set_size(_text_area, width, height);
    lv_textarea_set_text_selection(_text_area, true);
    lv_textarea_set_max_length(_text_area, 15);
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