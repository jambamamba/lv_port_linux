#include "leletabview.h"

#include "lelelabel.h"
#include "leletextbox.h"
#include "lelepos.h"

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

LeleTabView::Tabs::Tabs(const std::string &json_str, lv_obj_t *parent)
  : LeleBase(json_str, parent) {
    // for(const auto &pair: _tokens) {
    //     LOG(DEBUG, LVSIM, "Tabs token %s:%s\n", pair.first.c_str(), typeid(pair.second).name());
    //     _tab.emplace_back(std::move(pair.second));
    // }
}
int LeleTabView::Tabs::numTabs() const {
    int idx = 0;
    for(const auto &pair: _tokens) {
      if (std::holds_alternative<std::unique_ptr<LeleBase>>(pair.second)) {
        auto &value = std::get<std::unique_ptr<LeleBase>>(pair.second);
        if(pair.first == "tab") {
          LeleTabView::Tab *tab = dynamic_cast<LeleTabView::Tab*> (value.get());
          if(tab) {
            ++idx;        }        
          }
      }
    }
    return idx;
}
LeleTabView::Tab* LeleTabView::Tabs::getAt(int index) const {
    int idx = 0;
    for(const auto &pair: _tokens) {
      if (std::holds_alternative<std::unique_ptr<LeleBase>>(pair.second)) {
        auto &value = std::get<std::unique_ptr<LeleBase>>(pair.second);
        if(pair.first == "tab") {
          LeleTabView::Tab *tab = dynamic_cast<LeleTabView::Tab*> (value.get());
          if(tab) {
            if(index == idx) {
              return tab;
            }
            ++idx;
          }
        }
        // LOG(DEBUG, LVSIM, "Tabs token %s:%s\n", pair.first.c_str(), typeid(pair.second).name());
      }
    }
    return nullptr;
}

LeleTabView::Tab::Tab(const std::string &json_str, lv_obj_t *parent)
  : LeleBase(json_str, parent) {
}
LeleTabView::TabButton *LeleTabView::Tab::getTabButton() const {
    for(const auto &pair: _tokens) {
      if (std::holds_alternative<std::unique_ptr<LeleBase>>(pair.second)) {
        auto &value = std::get<std::unique_ptr<LeleBase>>(pair.second);
        if(pair.first == "tab_button") {
          LeleTabView::TabButton *ptr = dynamic_cast<LeleTabView::TabButton*> (value.get());
          if(ptr) {
            return ptr;
          }
        }
      }
    }
    return nullptr;
}
LeleTabView::TabContent *LeleTabView::Tab::getTabContent() const {
    for(const auto &pair: _tokens) {
      if (std::holds_alternative<std::unique_ptr<LeleBase>>(pair.second)) {
        auto &value = std::get<std::unique_ptr<LeleBase>>(pair.second);
        if(pair.first == "tab_content") {
          LeleTabView::TabContent *ptr = dynamic_cast<LeleTabView::TabContent*> (value.get());
          if(ptr) {
            return ptr;
          }
        }
      }
    }
    return nullptr;
}

LeleTabView::TabButton::TabButton(const std::string &json_str, lv_obj_t *parent)
  : LeleBase(json_str, parent) {
  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "name") {
        _name = value;
      }
      else if(key == "img") {
        _img = value;
      }
    }
  }
}
void LeleTabView::TabButton::setStyle(lv_obj_t *button, int active_tab_bgcolor, int active_tab_bottom_border_color) {
  _lv_obj = button;
  if(!_img.empty()) {
    lv_obj_t *logo = lv_image_create(button);
    lv_obj_add_flag(logo, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_image_set_src(logo, _lv_img_dsc_map.at(_img.c_str()));
    lv_obj_center(logo);
    lv_obj_t *label = lv_obj_get_child(button, 0);
    lv_label_set_text(label, "");
  }
  else {
    lv_obj_t *label = lv_obj_get_child(button, 0);
    lv_label_set_text(label, _name.c_str());
  }

  lv_obj_set_style_bg_color(button, lv_color_hex(active_tab_bgcolor), LV_PART_MAIN | LV_STATE_CHECKED);
  lv_obj_set_style_bg_color(button, lv_color_hex(active_tab_bgcolor), LV_PART_MAIN | LV_STATE_PRESSED);
  lv_obj_set_style_border_color(button, lv_color_hex(active_tab_bottom_border_color), LV_PART_MAIN | LV_STATE_CHECKED);
}
LeleTabView::TabContent::TabContent(const std::string &json_str, lv_obj_t *parent)
  : LeleBase(json_str, parent) {
}
lv_obj_t *LeleTabView::TabContent::createLvObj(lv_obj_t *parent) {
  for (const auto &[key, token]: _tokens) {
    LOG(DEBUG, LVSIM, "LeleTabView::TabContent::createLvObj: token with key: %s\n", key.c_str());
    if (std::holds_alternative<std::unique_ptr<LeleBase>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleBase>>(token);
      value->createLvObj(parent);
    }
  }
  return _lv_obj;
}

LeleTabView::LeleTabView(const std::string &json_str, lv_obj_t *parent)
  : LeleBase(json_str, parent) {
  int fgcolor, bgcolor, active_tab_color, active_tab_bottom_border_color;
  std::string title, subtitle, img;
  Tabs *tabs = nullptr;

  for (const auto &[key, token]: _tokens) {
    LOG(DEBUG, LVSIM, "Process token with key: %s\n", key.c_str());
    if (std::holds_alternative<std::unique_ptr<LeleBase>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleBase>>(token);
      if(key == "tabs") {
          tabs = dynamic_cast<Tabs*> (value.get());
      }
    }
    else if (std::holds_alternative<std::string>(token)) {
      std::string value = std::get<std::string>(token);
      if(key == "fgcolor") {
        fgcolor = std::stoi(value.c_str(), nullptr, 16);
      }
      else if(key == "bgcolor") {
        bgcolor = std::stoi(value.c_str(), nullptr, 16);
      }
      else if(key == "active_tab_color") {
        active_tab_color = std::stoi(value.c_str(), nullptr, 16);
      }
      else if(key == "active_tab_bottom_border_color") {
        active_tab_bottom_border_color = std::stoi(value.c_str(), nullptr, 16);
      }
      else if(key == "title") {
        title = value;
      }
      else if(key == "subtitle") {
        subtitle = value;
      }
      else if(key == "img") {
        img = value;
      }
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

  for(int idx = 0; idx < tabs->numTabs(); ++idx) {
    LeleTabView::Tab *tab = tabs->getAt(idx);
      tab->setLvObj(lv_tabview_add_tab(_lv_obj, tab->getTabButton()->name().c_str()));
      lv_obj_t *button = lv_obj_get_child(tabview_header, idx);
      tab->getTabButton()->setStyle(button, active_tab_color, active_tab_bottom_border_color);
      tab->getTabContent()->createLvObj(tab->getLvObj());
  }

  lv_obj_t *logo = setTabViewImg(tabview_header, img);
  lv_obj_t *label = setTabViewTitle(tabview_header, title);
  lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);
  label = setTabViewSubTitle(tabview_header, subtitle);
  lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_BOTTOM, 10, 0);
}

lv_obj_t *LeleTabView::setTabViewImg(lv_obj_t *tabview_header, const std::string &img) {
    lv_obj_set_style_pad_left(tabview_header, LV_HOR_RES / 2, 0);
    lv_obj_t *logo = lv_image_create(tabview_header);
    lv_obj_add_flag(logo, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_image_set_src(logo, _lv_img_dsc_map.at(img));
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
    lv_style_init(&_style_subtitle);
    lv_style_set_text_opa(&_style_subtitle, LV_OPA_50);
    lv_obj_add_style(label, &_style_subtitle, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_label_set_text(label, subtitle.c_str());
    return label;
}

void LeleTabView::tabViewDeleteEventCb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    LeleTabView *pthis = (LeleTabView*)e->user_data;

    if(code == LV_EVENT_DELETE) {
        //osm lv_style_reset(&pthis->_style_subtitle);
        //osm lv_style_reset(&pthis->_style_title);
    }
}

// void LeleTabView::Tab::setTabButton(lv_obj_t *button, int active_tab_bgcolor, int active_tab_bottom_border_color) {
  // _tab_button = button;
  // lv_obj_t *logo = lv_image_create(button);
  // lv_obj_add_flag(logo, LV_OBJ_FLAG_IGNORE_LAYOUT);
  // lv_image_set_src(logo, _lv_img_dsc_map.at(_img.c_str()));
  // lv_obj_center(logo);
  // lv_obj_t *label = lv_obj_get_child(button, 0);
  // lv_label_set_text(label, "");

  // lv_obj_set_style_bg_color(button, lv_color_hex(active_tab_bgcolor), LV_PART_MAIN | LV_STATE_CHECKED);
  // lv_obj_set_style_bg_color(button, lv_color_hex(active_tab_bgcolor), LV_PART_MAIN | LV_STATE_PRESSED);
  // lv_obj_set_style_border_color(button, lv_color_hex(active_tab_bottom_border_color), LV_PART_MAIN | LV_STATE_CHECKED);
// }

// void LeleTabView::Tab::setTabContent(lv_obj_t *tab_content) {
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
// }