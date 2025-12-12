#include "lelelabel.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleLabel::LeleLabel(const std::string &json_str)
  : LeleObject(json_str) {
  _class_name = __func__;
  fromJson(json_str);
}

bool LeleLabel::fromJson(const std::string &json_str) {
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

lv_obj_t *LeleLabel::createLvObj(LeleObject *lele_parent, lv_obj_t *lv_obj) {

  _lv_obj = LeleObject::createLvObj(lele_parent,
    lv_label_create(lele_parent->getLvObj()));

  lv_label_set_text(_lv_obj, _text.c_str());
  return _lv_obj;
}

void LeleLabel::setText(const std::string &text) {
  _text = text;
  lv_label_set_text(_lv_obj, _text.c_str());
}

std::string LeleLabel::getText() const { 
  return _text; 
}
