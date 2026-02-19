#include "lelerollerview.h"

#include <tr/tr.h>

LOG_CATEGORY(LVSIM, "LVSIM");

LeleRollerView::LeleRollerView(LeleObject *parent, const std::string &json_str)
  : LeleObject(parent, json_str) {
  _class_name = __func__;
  fromJson(json_str);
}

bool LeleRollerView::fromJson(const std::string &json_str) {
  for (const auto &[key, token]: _nodes) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "text") {
        _text = value;
      }
    }
  }
  return true;
}

lv_obj_t *LeleRollerView::createLvObj(LeleObject *lele_parent, lv_obj_t *lv_obj) {

  _lv_obj = LeleObject::createLvObj(lele_parent,
    lv_roller_create(lele_parent->getLvObj()));

  lele_set_translatable_text([this](){
    lv_roller_set_options(_lv_obj,
                          "January\n"
                          "February\n"
                          "March\n"
                          "April\n"
                          "May\n"
                          "June\n"
                          "July\n"
                          "August\n"
                          "September\n"
                          "October\n"
                          "November\n"
                          "December",
                          LV_ROLLER_MODE_INFINITE);    
  });
  lv_roller_set_visible_row_count(_lv_obj, 4);
  lv_obj_center(_lv_obj);
  return _lv_obj;
}

bool LeleRollerView::eventCallback(LeleEvent &&e) {

  // LL(DEBUG, LVSIM) << "LeleRollerView::eventCallback " <<
  //   ", event_code: " << e.getLvEvent()->code <<
  //   " " << lv_event_code_get_name(e.getLvEvent()->code);

  switch(e.getLvEvent()->code) {
    case LV_EVENT_CLICKED:
    case LV_EVENT_LONG_PRESSED:
    case LV_EVENT_LONG_PRESSED_REPEAT:
    case LV_EVENT_SCROLL: {
      break;
    }
    case LV_EVENT_VALUE_CHANGED: {
      break;
    }
    default: {
      break;
    }
  }
  return true;
}

void LeleRollerView::setText(const std::string &text) {
  _text = text;
  // lele_set_translatable_text([this](){
  //   lv_label_set_text(_lv_obj, tr(_text).c_str());
  // });
}

std::string LeleRollerView::getText() const { 
  return _text; 
}
