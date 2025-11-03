#include "lelemessagebox.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleMessageBox::LeleMessageBox(const std::string &json_str)
  : LeleBase(json_str) {

  _class_name = __func__ ;//typeid(this).name();
  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "title") {
        _title = value;
      }
      else if(key == "text") {
        _text = value;
      }
    }
    // else if (std::holds_alternative<std::unique_ptr<LeleBase>>(token)) {
    //   auto &value = std::get<std::unique_ptr<LeleBase>>(token);
    //   value->createLvObj(this);//osm
    //   // if(key == "button") {
    //   //   _btn = std::make_unique<LeleButtons::LeleButton>(value);
    //   // }
    // }
  }
}

lv_obj_t *LeleMessageBox::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {

  _lv_obj = LeleBase::createLvObj(lele_parent,
    lv_msgbox_create(nullptr));

  lv_msgbox_add_title(_lv_obj, _title.c_str());
  lv_msgbox_add_text(_lv_obj, _text.c_str());

  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::unique_ptr<LeleBase>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleBase>>(token);
      lv_obj_t *btn = value->createLvObj(nullptr, lv_msgbox_add_close_button(_lv_obj));
      // value->setStyle(btn);//osm todo: the style is not set if class_name is set in the json

      // lv_msgbox_add_close_button(mbox1);
      // btn = lv_msgbox_add_footer_button(mbox1, "Apply");
      // lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);
      // btn = lv_msgbox_add_footer_button(mbox1, "Cancel");
      // lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);    
      }
  }
  return _lv_obj;
}

void LeleMessageBox::setText(const std::string &text) {
  _text = text;
  lv_label_set_text(_lv_obj, _text.c_str());
}

std::string LeleMessageBox::getText() const { 
  return _text; 
}
