#include "lelestackview.h"

#include "lelelabel.h"
#include "leletextbox.h"
#include "lelestyle.h"
#include "leleview.h"

#include <lv_image_converter/mainlib.h>

LOG_CATEGORY(LVSIM, "LVSIM");

LeleStackView::LeleStackView(const std::string &json_str)
  : LeleBase(json_str) {
    _class_name = __func__ ;//
  for (const auto &[key, token]: _tokens) {
    LOG(DEBUG, LVSIM, "Process token with key: %s\n", key.c_str());
    if (std::holds_alternative<std::unique_ptr<LeleBase>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleBase>>(token);
      if(key == "views") {
          _views = dynamic_cast<LeleViews*> (value.get());
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
        // printf("@@@ value:%s\n", value.c_str());
        // printf("@@@@ _active_tab_bottom_border_type:%i\n",_active_tab_bottom_border_type);
        // printf("@@@@ _active_tab_bottom_border_width:%i\n",_active_tab_bottom_border_width);
        // printf("@@@@ _active_tab_bottom_border_color:0x%x\n",_active_tab_bottom_border_color);
      }
      else if(key == "tabbar_height") {
        _tabbar_height = std::stoi(value.c_str(), nullptr, 10);
      }
    }
  }
}

lv_obj_t *LeleStackView::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {

  setParent(lele_parent);
  _lv_obj = lv_tabview_create(lele_parent->getLvObj());

  lv_tabview_set_tab_bar_size(_lv_obj, _tabbar_height);
  lv_obj_add_event_cb(_lv_obj, statckViewDeleteEventCb, LV_EVENT_DELETE, this);

  const lv_font_t *font_normal = &lv_font_montserrat_16;
  lv_obj_set_style_text_font(_lv_obj, font_normal, 0);
  lv_obj_set_style_text_color(_lv_obj, lv_color_hex(_lele_style->fgColor()), LV_PART_MAIN);
  lv_obj_set_style_bg_color(_lv_obj, lv_color_hex(_lele_style->bgColor()), LV_PART_MAIN);
  
  lv_obj_t *tabview_content = lv_tabview_get_content(_lv_obj);
  lv_obj_t *tabview_header = lv_tabview_get_tab_bar(_lv_obj);
  lv_obj_set_style_text_color(tabview_header, lv_color_hex(_lele_style->fgColor()), LV_PART_MAIN);
  lv_obj_set_style_bg_color(tabview_header, lv_color_hex(_lele_style->bgColor()), LV_PART_MAIN);

  _views->createLvObj(this);
  _views->setLvObj(_lv_obj);
  for(int idx = 0; idx < _views->count(); ++idx) {
    LeleView *view = _views->getAt(idx);
    view->createLvObj(_views);
    // view->getTabContent()->createLvObj(view);//osm: create child view which will be the content

    lv_obj_t *button = lv_obj_get_child(tabview_header, idx);
    view->setLvObj(button);
    LeleViewHeader *view_header = dynamic_cast<LeleViewHeader*>(getLeleObj("view_header"));
    if(view_header) {
      view_header->createLvObj(view);
    }
    lv_obj_set_style_bg_color(button, lv_color_hex(_active_tab_bgcolor), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(button, lv_color_hex(_active_tab_bgcolor), LV_PART_MAIN | LV_STATE_PRESSED);
    if(_active_tab_bottom_border_type == LeleStyle::BorderTypeE::Solid) {
      lv_obj_set_style_border_color(button, lv_color_hex(_active_tab_bottom_border_color), LV_PART_MAIN | LV_STATE_CHECKED);
      lv_obj_set_style_border_width(button, _active_tab_bottom_border_width, LV_PART_MAIN | LV_STATE_CHECKED);
    }
  }

  lv_obj_t *logo = setStackViewImg(tabview_header, _img);
  lv_obj_t *label = setStackViewTitle(tabview_header, _title);
  lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);
  label = setStackViewSubTitle(tabview_header, _subtitle);
  lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_BOTTOM, 10, 0);

  return _lv_obj;
}

lv_obj_t *LeleStackView::setStackViewImg(lv_obj_t *tabview_header, const std::string &img) {//osm: same as setTabViewImg
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

lv_obj_t *LeleStackView::setStackViewTitle(lv_obj_t *tabview_header, const std::string &title) {//osm: same as setTabViewTitle
    lv_obj_t *label = lv_label_create(tabview_header);
    lv_style_init(&_style_title);
    const lv_font_t *font_large = &lv_font_montserrat_24;
    lv_style_set_text_font(&_style_title, font_large);
    lv_obj_add_style(label, &_style_title, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_label_set_text_fmt(label, "%s", title.c_str());//"LVGL v%d.%d.%d", lv_version_major(), lv_version_minor(), lv_version_patch());
    return label;
}

lv_obj_t *LeleStackView::setStackViewSubTitle(lv_obj_t *tabview_header, const std::string &subtitle) {//osm: same as setTabViewSubTitle
    lv_obj_t *label = lv_label_create(tabview_header);
    lv_style_init(&_style_subtitle);
    lv_style_set_text_opa(&_style_subtitle, LV_OPA_50);
    lv_obj_add_style(label, &_style_subtitle, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_label_set_text(label, subtitle.c_str());
    return label;
}

void LeleStackView::statckViewDeleteEventCb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    LeleStackView *pthis = (LeleStackView*)e->user_data;

    if(code == LV_EVENT_DELETE) {
        //osm lv_style_reset(&pthis->_style_subtitle);
        //osm lv_style_reset(&pthis->_style_title);
    }
}
