#include "leletabview.h"

#include "lelelabel.h"
#include "leletextbox.h"
#include "lelepos.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleTabView::Tabs::Tabs(const std::string &json_str)
  : LeleBase(json_str) {
}
lv_obj_t *LeleTabView::Tabs::createLvObj(lv_obj_t *lv_obj, LeleBase *lele_parent) {
  _lele_parent = lele_parent;
  _lv_obj = lv_obj;
  return _lv_obj;
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

LeleTabView::Tab::Tab(const std::string &json_str)
  : LeleBase(json_str) {
}
lv_obj_t *LeleTabView::Tab::createLvObj(lv_obj_t *lv_obj, LeleBase *lele_parent) {
  _lele_parent = lele_parent;
  _lv_obj = lv_obj;
  return _lv_obj;
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

LeleTabView::TabButton::TabButton(const std::string &json_str)
  : LeleBase(json_str) {
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
lv_obj_t *LeleTabView::TabButton::createLvObj(lv_obj_t *lv_obj, LeleBase *lele_parent) {
  if(!_img.empty()) {
    lv_obj_t *logo = lv_image_create(lv_obj);
    lv_obj_add_flag(logo, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_image_set_src(logo, _lv_img_dsc_map.at(_img.c_str()));
    lv_obj_center(logo);
    lv_obj_t *label = lv_obj_get_child(lv_obj, 0);
    lv_label_set_text(label, "");
  }
  else {
    lv_obj_t *label = lv_obj_get_child(lv_obj, 0);
    lv_label_set_text(label, _name.c_str());
  }

  _lele_parent = lele_parent;
  _lv_obj = lv_obj;
  return _lv_obj;
}
LeleTabView::TabContent::TabContent(const std::string &json_str)
  : LeleBase(json_str) {
}
lv_obj_t *LeleTabView::TabContent::createLvObj(lv_obj_t *lv_parent, LeleBase *lele_parent) {
  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::unique_ptr<LeleBase>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleBase>>(token);
      value->createLvObj(lv_parent, this);
    }
  }
  _lele_parent = lele_parent;
  return _lv_obj;
}

LeleTabView::LeleTabView(const std::string &json_str)
  : LeleBase(json_str) {

  for (const auto &[key, token]: _tokens) {
    LOG(DEBUG, LVSIM, "Process token with key: %s\n", key.c_str());
    if (std::holds_alternative<std::unique_ptr<LeleBase>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleBase>>(token);
      if(key == "tabs") {
          _tabs = dynamic_cast<Tabs*> (value.get());
      }
    }
    else if (std::holds_alternative<std::string>(token)) {
      std::string value = std::get<std::string>(token);
      if(key == "title") {
        _title = value;
      }
      else if(key == "subtitle") {
        _subtitle = value;
      }
      else if(key == "img") {
        _img = value;
      }
      else if(key == "active_tab_bgcolor") {
        _active_tab_bgcolor = LelePos::parseColorCode(value);
      }
      else if(key == "active_tab_bottom_border_color") {
        _active_tab_bottom_border_color = LelePos::parseColorCode(value);
      }
      else if(key == "tabbar_height") {
        _tabbar_height = std::stoi(value.c_str(), nullptr, 10);
      }
    }
  }
}

lv_obj_t *LeleTabView::createLvObj(lv_obj_t *lv_parent, LeleBase *lele_parent) {

  _lv_obj = lv_tabview_create(lv_parent);

  lv_tabview_set_tab_bar_size(_lv_obj, _tabbar_height);
  lv_obj_add_event_cb(_lv_obj, tabViewDeleteEventCb, LV_EVENT_DELETE, this);

  const lv_font_t *font_normal = &lv_font_montserrat_16;
  lv_obj_set_style_text_font(_lv_obj, font_normal, 0);
  lv_obj_set_style_text_color(_lv_obj, lv_color_hex(_pos->fgColor()), LV_PART_MAIN);
  lv_obj_set_style_bg_color(_lv_obj, lv_color_hex(_pos->bgColor()), LV_PART_MAIN);
  
  lv_obj_t *tabview_content = lv_tabview_get_content(_lv_obj);
  lv_obj_t *tabview_header = lv_tabview_get_tab_bar(_lv_obj);
  lv_obj_set_style_text_color(tabview_header, lv_color_hex(_pos->fgColor()), LV_PART_MAIN);
  lv_obj_set_style_bg_color(tabview_header, lv_color_hex(_pos->bgColor()), LV_PART_MAIN);

  _tabs->createLvObj(nullptr, this);
  for(int idx = 0; idx < _tabs->numTabs(); ++idx) {
    LeleTabView::Tab *tab = _tabs->getAt(idx);
    tab->createLvObj(lv_tabview_add_tab(_lv_obj, tab->getTabButton()->name().c_str()), _tabs);

    lv_obj_t *button = lv_obj_get_child(tabview_header, idx);
    tab->getTabButton()->createLvObj(button, tab);
    lv_obj_set_style_bg_color(button, lv_color_hex(_active_tab_bgcolor), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(button, lv_color_hex(_active_tab_bgcolor), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_border_color(button, lv_color_hex(_active_tab_bottom_border_color), LV_PART_MAIN | LV_STATE_CHECKED);

    tab->getTabContent()->createLvObj(tab->getLvObj(), tab);
  }

  lv_obj_t *logo = setTabViewImg(tabview_header, _img);
  lv_obj_t *label = setTabViewTitle(tabview_header, _title);
  lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);
  label = setTabViewSubTitle(tabview_header, _subtitle);
  lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_BOTTOM, 10, 0);

  _lele_parent = lele_parent;
  return _lv_obj;
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
