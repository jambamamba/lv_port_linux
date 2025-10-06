#include "leletabview.h"

#include "lelelabel.h"
#include "leletextbox.h"
#include "lelestyle.h"
#include "leleview.h"

#include <lv_image_converter/mainlib.h>

LOG_CATEGORY(LVSIM, "LVSIM");

LeleTabView::Tabs::Tabs(const std::string &json_str)
  : LeleBase(json_str) {
    _class_name = __func__ ;//
}
lv_obj_t *LeleTabView::Tabs::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {
  setParent(lele_parent);
  return _lv_obj;
}

int LeleTabView::Tabs::count() const {
    return getChildren().size();
}
LeleTabView::Tab* LeleTabView::Tabs::getAt(int index) const {
    auto list = getChildren();
    if(list.size() > index) {
        return list.at(index);
    }
    return nullptr;
}
std::vector<LeleTabView::Tab*> LeleTabView::Tabs::getChildren() const {
    std::vector<LeleTabView::Tab*> ret;
    for(const auto &pair: _tokens) {
      if (std::holds_alternative<std::unique_ptr<LeleBase>>(pair.second)) {
        auto &value = std::get<std::unique_ptr<LeleBase>>(pair.second);
        if(pair.first == "tab") {
          LeleTabView::Tab *tab = dynamic_cast<LeleTabView::Tab*> (value.get());
          if(tab) {
              ret.push_back(tab);
          }
        }
        // LOG(DEBUG, LVSIM, "Tabs token %s:%s\n", pair.first.c_str(), typeid(pair.second).name());
      }
    }
    return ret;
}

LeleTabView::Tab::Tab(const std::string &json_str)
  : LeleBase(json_str) {
    _class_name = __func__ ;//
}
lv_obj_t *LeleTabView::Tab::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {
  setParent(lele_parent);
  _lv_obj = lv_tabview_add_tab(lele_parent->getLvObj(), getTabHeader()->name().c_str());
  return _lv_obj;
}
LeleTabView::TabHeader *LeleTabView::Tab::getTabHeader() const {
  return dynamic_cast<LeleTabView::TabHeader*>(getLeleObj("tab_header"));
}
LeleTabView::TabContent *LeleTabView::Tab::getTabContent() const {
  return dynamic_cast<LeleTabView::TabContent*>(getLeleObj("tab_content"));
}


LeleTabView::TabHeader::TabHeader(const std::string &json_str)
  : LeleBase(json_str) {
    _class_name = __func__ ;//
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
lv_obj_t *LeleTabView::TabHeader::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {
  if(!_img.empty()) {
    lv_obj_t *logo = lv_image_create(lele_parent->getLvObj());
    lv_obj_add_flag(logo, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_image_set_src(logo, _lv_img_dsc_map.at(_img.c_str()));
    // _img_dsc = generateImgDsc((std::string("/repos/lv_port_linux/res/") + _img).c_str());//osm
    // if(_img_dsc) {
    //   lv_image_set_src(logo, _img_dsc.value().get());
    // }
    lv_obj_center(logo);
    lv_obj_t *label = lv_obj_get_child(lele_parent->getLvObj(), 0);
    lv_label_set_text(label, "");
  }
  else {
    lv_obj_t *label = lv_obj_get_child(lele_parent->getLvObj(), 0);
    lv_label_set_text(label, _name.c_str());
  }

  setParent(lele_parent);
  _lv_obj = lele_parent->getLvObj();
  return _lv_obj;
}


LeleTabView::TabContent::TabContent(const std::string &json_str)
  : LeleBase(json_str) {
    _class_name = __func__ ;//
}
lv_obj_t *LeleTabView::TabContent::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {
  setParent(lele_parent);
  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::unique_ptr<LeleBase>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleBase>>(token);
      value->createLvObj(lele_parent);
    }
  }
  setParent(lele_parent);
  return _lv_obj;
}

LeleTabView::LeleTabView(const std::string &json_str)
  : LeleBase(json_str) {
    _class_name = __func__ ;//
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
        _active_tab_bgcolor = LeleStyle::parseColorCode(value);
      }
      else if(key == "active_tab_bottom_border") {
        std::tie(_active_tab_bottom_border_type,_active_tab_bottom_border_width,_active_tab_bottom_border_color) = 
          LeleStyle::parseBorder(value);
        // LOG(DEBUG, LVSIM, "value:%s\n", value.c_str());
        // LOG(DEBUG, LVSIM, "_active_tab_bottom_border_type:%i\n",_active_tab_bottom_border_type);
        // LOG(DEBUG, LVSIM, "_active_tab_bottom_border_width:%i\n",_active_tab_bottom_border_width);
        // LOG(DEBUG, LVSIM, "_active_tab_bottom_border_color:0x%x\n",_active_tab_bottom_border_color);
      }
      else if(key == "tabbar_height") {
        _tabbar_height = std::stoi(value.c_str(), nullptr, 10);
      }
      else if(key == "tabbar_location") {
        _tabbar_location = (value == "bottom") ? TabBarLocationE::Bottom : TabBarLocationE::Top;
      }
    }
  }
}

lv_obj_t *LeleTabView::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {

  setParent(lele_parent);
  _lv_obj = lv_tabview_create(lele_parent->getLvObj());
  lv_tabview_set_tab_bar_position(_lv_obj, _tabbar_location == TabBarLocationE::Bottom ? LV_DIR_BOTTOM : LV_DIR_TOP);

  lv_tabview_set_tab_bar_size(_lv_obj, _tabbar_height);
  lv_obj_add_event_cb(_lv_obj, tabViewDeleteEventCb, LV_EVENT_DELETE, this);

  const lv_font_t *font_normal = &lv_font_montserrat_16;
  lv_obj_set_style_text_font(_lv_obj, font_normal, 0);
  auto fgcolor =_lele_styles.getValue("fgcolor");
  if(fgcolor) {
    lv_obj_set_style_text_color(_lv_obj, lv_color_hex(std::get<int>(fgcolor.value())), LV_PART_MAIN);
  }
  auto bgcolor =_lele_styles.getValue("bgcolor");
  if(bgcolor) {
    lv_obj_set_style_bg_color(_lv_obj, lv_color_hex(std::get<int>(bgcolor.value())), LV_PART_MAIN);
  }
  
  lv_obj_t *tabview_content = lv_tabview_get_content(_lv_obj);
  lv_obj_t *tabview_header = lv_tabview_get_tab_bar(_lv_obj);
  if(fgcolor) {
    lv_obj_set_style_text_color(tabview_header, lv_color_hex(std::get<int>(fgcolor.value())), LV_PART_MAIN);
  }
  if(bgcolor) {
    lv_obj_set_style_bg_color(tabview_header, lv_color_hex(std::get<int>(bgcolor.value())), LV_PART_MAIN);
  }

  if(_tabs){
    _tabs->createLvObj(this);
    _tabs->setLvObj(_lv_obj);
    for(int idx = 0; idx < _tabs->count(); ++idx) {
      LeleTabView::Tab *tab = _tabs->getAt(idx);
      tab->createLvObj(_tabs);
      tab->getTabContent()->createLvObj(tab);

      lv_obj_t *button = lv_obj_get_child(tabview_header, idx);
      tab->setLvObj(button);
      tab->getTabHeader()->createLvObj(tab);
      lv_obj_set_style_bg_color(button, lv_color_hex(_active_tab_bgcolor), LV_PART_MAIN | LV_STATE_CHECKED);
      lv_obj_set_style_bg_color(button, lv_color_hex(_active_tab_bgcolor), LV_PART_MAIN | LV_STATE_PRESSED);
      if(_active_tab_bottom_border_type == LeleStyle::BorderTypeE::Solid) {
        lv_obj_set_style_border_color(button, lv_color_hex(_active_tab_bottom_border_color), LV_PART_MAIN | LV_STATE_CHECKED);
        lv_obj_set_style_border_width(button, _active_tab_bottom_border_width, LV_PART_MAIN | LV_STATE_CHECKED);
      }
    }
  }

  if(tabview_header){
    lv_obj_t *logo = setTabViewImg(tabview_header, _img);
    lv_obj_t *label = setTabViewTitle(tabview_header, _title);
    lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);
    label = setTabViewSubTitle(tabview_header, _subtitle);
    lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_BOTTOM, 10, 0);
  }

  return _lv_obj;
}

lv_obj_t *LeleTabView::setTabViewImg(lv_obj_t *tabview_header, const std::string &img) {
    lv_obj_set_style_pad_left(tabview_header, LV_HOR_RES / 2, 0);
    lv_obj_t *logo = lv_image_create(tabview_header);
    lv_obj_add_flag(logo, LV_OBJ_FLAG_IGNORE_LAYOUT);
    // lv_image_set_src(logo, _lv_img_dsc_map.at(img));
    _logo = generateImgDsc((std::string("/repos/lv_port_linux/res/") + img).c_str());//osm
    if(_logo) {
      lv_image_set_src(logo, _logo.value().get());
    }
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
