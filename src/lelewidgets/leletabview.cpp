#include "leletabview.h"

#include "lelelabel.h"
#include "leletextbox.h"
#include "lelepos.h"
#include "lelewidgetfactory.h"

LOG_CATEGORY(LVSIM, "LVSIM");

// LeleTabView::LeleTabView(
//   const std::string &title,
//   const std::string &subtitle,
//   const std::string &logo_img,
//   const std::string &fgcolor_str,
//   const std::string &bgcolor_str,
//   const std::string &active_tab_bgcolor_str,
//   const std::string &active_tab_bottom_border_color_str,
//   const std::vector<std::string> &tabs_json_str
// ) : LeleBase() {

//     int fgcolor = std::stoi(fgcolor_str, nullptr, 16);
//     int bgcolor = std::stoi(bgcolor_str, nullptr, 16);
//     int active_tab_color = std::stoi(active_tab_bgcolor_str, nullptr, 16);
//     int active_tab_bottom_border_color = std::stoi(active_tab_bottom_border_color_str, nullptr, 16);

    // constexpr int32_t tab_h = 75;
    // _lv_obj = lv_tabview_create(lv_screen_active());
    // lv_tabview_set_tab_bar_size(_lv_obj, tab_h);
    // lv_obj_add_event_cb(_lv_obj, tabViewDeleteEventCb, LV_EVENT_DELETE, this);

    // const lv_font_t *font_normal = &lv_font_montserrat_16;
    // lv_obj_set_style_text_font(lv_screen_active(), font_normal, 0);
    // lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(fgcolor), LV_PART_MAIN);
    // lv_obj_set_style_bg_color(_lv_obj, lv_color_hex(bgcolor), LV_PART_MAIN);
    
    // lv_obj_t *tabview_content = lv_tabview_get_content(_lv_obj);
    // lv_obj_t *tabview_header = lv_tabview_get_tab_bar(_lv_obj);
    // lv_obj_set_style_text_color(tabview_header, lv_color_hex(fgcolor), LV_PART_MAIN);
    // lv_obj_set_style_bg_color(tabview_header, lv_color_hex(bgcolor), LV_PART_MAIN);

//     int idx = 0;
//     for(const auto &tab_json_str: tabs_json_str) {
//       ++idx;
//     }

    // lv_obj_t *logo = setTabViewImg(tabview_header, logo_img);
    // lv_obj_t *label = setTabViewTitle(tabview_header, title);
    // lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);
    // label = setTabViewSubTitle(tabview_header, subtitle);
    // lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_BOTTOM, 10, 0);
// }

LeleTabView::Tabs::Tabs(const std::string &json_str) {
  auto tokens = LeleWidgetFactory::fromJson(json_str);
  for (const auto &[key, value]: tokens) {
    if(key == "tabs") {
      _tab = LeleWidgetFactory::fromJson(value);
    }
  }
}
LeleTabView::Tab::Tab(const std::string &json_str) {
  auto tokens = LeleWidgetFactory::fromJson(json_str);
  for (const auto &[key, value]: tokens) {
    if(key == "tab_button") {
      _tab_button = LeleWidgetFactory::fromJson(value);
    }
    else if(key == "tab_content") {
      _tab_content = LeleWidgetFactory::fromJson(value);
    }
  }
}
LeleTabView::TabButton::TabButton(const std::string &json_str) {
  auto tokens = LeleWidgetFactory::fromJson(json_str);
  for (const auto &[key, value]: tokens) {
    if(key == "name") {
      _name = value;
    }
    else if(key == "img") {
      _img = value;
    }
  }
}
LeleTabView::TabContent::TabContent(const std::string &json_str) {
  _widget = LeleWidgetFactory::fromJson(json_str);
}
LeleTabView::LeleTabView(const std::string &json_str) {
  int fgcolor, bgcolor, active_tab_color, active_tab_bottom_border_color;
  const std::string title, subtitle, logo_img;

  auto tokens = LeleWidgetFactory::fromJson(json_str);
  for (const auto &[key, value]: tokens) {
    if(key == "tabs") {
      for(const auto &obj:value){
        if (std::holds_alternative<std::unique_ptr<LeleBase>>(obj)) {
          _tabs.emplace_back(LeleWidgetFactory::fromJson(obj));
      }
    }
    else if(key == "fgcolor") {
      fgcolor = std::stoi(value, nullptr, 16);
    }
    else if(key == "bgcolor") {
      bgcolor = std::stoi(value, nullptr, 16);
    }
    else if(key == "active_tab_color") {
      active_tab_color = std::stoi(value, nullptr, 16);
    }
    else if(key == "active_tab_bottom_border_color") {
      active_tab_bottom_border_color = std::stoi(value, nullptr, 16);
    }
    else if(key == "title") {
      title = value;
    }
    else if(key == "subtitle") {
      subtitle = value;
    }
    else if(key == "logo_img") {
      logo_img = value;
    }
  }

  //osm todo: get the hard coded values from config.json:
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
    lv_label_set_text_fmt(label, "%s", title.c_str());//"LVGL v%d.%d.%d", lv_version_major(), lv_version_minor(), lv_version_patch());
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

// std::optional<std::unique_ptr<LeleTabView>> LeleTabView::fromJson(const cJSON *tabview) {

//     const cJSON *title = objFromJson(tabview, "title");
//     if(!title) {
//       LOG(WARNING, LVSIM, "tabview is missing title\n");
//     }
//     LOG(DEBUG, LVSIM, "%s:%s\n", title->string, title->valuestring);
//     std::string title_str = title->valuestring;

//     const cJSON *subtitle = objFromJson(tabview, "subtitle");
//     if(!subtitle) {
//       LOG(WARNING, LVSIM, "tabview is missing subtitle\n");
//     }
//     LOG(DEBUG, LVSIM, "%s:%s\n", subtitle->string, subtitle->valuestring);
//     std::string subtitle_str = subtitle->valuestring;

//     const cJSON *img = objFromJson(tabview, "img");
//     if(!img) {
//       LOG(WARNING, LVSIM, "tabview is missing img\n");
//     }
//     LOG(DEBUG, LVSIM, "%s:%s\n", img->string, img->valuestring);
//     std::string img_str = img->valuestring;
    
//     const cJSON *fgcolor = objFromJson(tabview, "fgcolor");
//     if(!fgcolor) {
//       LOG(WARNING, LVSIM, "tabview is missing fgcolor\n");
//     }
//     LOG(DEBUG, LVSIM, "%s:%s\n", fgcolor->string, fgcolor->valuestring);
//     std::string fgcolor_str = fgcolor->valuestring;

//     const cJSON *bgcolor = objFromJson(tabview, "bgcolor");
//     if(!bgcolor) {
//       LOG(WARNING, LVSIM, "tabview is missing bgcolor\n");
//     }
//     LOG(DEBUG, LVSIM, "%s:%s\n", bgcolor->string, bgcolor->valuestring);
//     std::string bgcolor_str = bgcolor->valuestring;

//     const cJSON *active_tab_bgcolor = objFromJson(tabview, "active_tab_bgcolor");
//     if(!bgcolor) {
//       LOG(WARNING, LVSIM, "tabview is missing active_tab_bgcolor\n");
//     }
//     LOG(DEBUG, LVSIM, "%s:%s\n", active_tab_bgcolor->string, active_tab_bgcolor->valuestring);
//     std::string active_tab_bgcolor_str = active_tab_bgcolor->valuestring;

//     const cJSON *active_tab_bottom_border_color = objFromJson(
//       tabview, "active_tab_bottom_border_color");
//     if(!bgcolor) {
//       LOG(WARNING, LVSIM, "tabview is missing active_tab_bottom_border_color\n");
//     }
//     LOG(DEBUG, LVSIM, "%s:%s\n", active_tab_bottom_border_color->string, active_tab_bottom_border_color->valuestring);
//     std::string active_tab_bottom_border_color_str = active_tab_bottom_border_color->valuestring;

//     const cJSON *json_tabs = objFromJson(tabview, "tabs");
//     if(!json_tabs) {
//         LOG(WARNING, LVSIM, "tabviewtabs is missing tabs\n");
//         return std::nullopt;
//     }
//     std::vector<std::unique_ptr<LeleTabView::Tab>> tabs;
//     if(cJSON_IsArray(json_tabs)) {
//         cJSON *json_tab = nullptr;
//         cJSON_ArrayForEach(json_tab, json_tabs) {
//             tabs.emplace_back(
//               LeleTabView::Tab::fromJson(json_tab));
//         }
//     }

//     return std::make_unique<LeleTabView>(
//       title_str, 
//       subtitle_str, 
//       img_str, 
//       fgcolor_str, 
//       bgcolor_str, 
//       active_tab_bgcolor_str, 
//       active_tab_bottom_border_color_str, 
//       std::move(tabs));
// }

// std::unique_ptr<LeleTabView::Tab> LeleTabView::Tab::fromJson(const cJSON *json_tab) {
//   std::string name; 
//   std::string img;
//   std::string content;
//   cJSON *item = nullptr;
//   cJSON_ArrayForEach(item, json_tab) {
//       LOG(DEBUG, LVSIM, "%s:%s\n", item->string, item->valuestring);
//       if(strcmp(item->string, "name")==0) {
//           name = cJSON_GetStringValue(item);
//       }
//       else if(strcmp(item->string, "img")==0) {
//           img = cJSON_GetStringValue(item);
//       }
//       else if(strcmp(item->string, "tab_content")==0) {
//           content = cJSON_Print(item);//This has bug: cJSON_Duplicate(item, true);//dont forget to cJSON_Delete
//       }
//   }
//   return std::make_unique<LeleTabView::Tab>(name, img, content);
// }

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

void LeleTabView::Tab::setTabContent(lv_obj_t *tab_content) {
  // if(_json.empty() || !tab_content) {
  //   return;
  // }
  // cJSON *items = cJSON_Parse(_json.c_str());
  // cJSON *item = nullptr;
  // cJSON_ArrayForEach(item, items) {
  //     if(strcmp(item->string, "label") == 0) {
  //       addChild(LeleBase::fromJson<LeleLabel>(item, _lv_obj, lv_obj_get_width(tab_content), lv_obj_get_height(tab_content)));
  //     }
  //     if(strcmp(item->string, "textbox") == 0) {
  //       addChild(LeleBase::fromJson<LeleTextbox>(item, _lv_obj, lv_obj_get_width(tab_content), lv_obj_get_height(tab_content)));
  //     }
  // }
}