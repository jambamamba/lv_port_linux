#include "lelelabel.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleLabel::LeleLabel(const std::string &json_str)
  : LeleBase(json_str) {

  _class_name = __func__ ;//typeid(this).name();
  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "text") {
        _text = value;
      }
    }
  }
}

lv_obj_t *LeleLabel::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {

  _lv_obj = LeleBase::createLvObj(lele_parent,
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
