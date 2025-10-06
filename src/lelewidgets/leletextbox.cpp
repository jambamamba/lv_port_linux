#include "leletextbox.h"


LOG_CATEGORY(LVSIM, "LVSIM");

LeleTextbox::LeleTextbox(const std::string &json_str)
  : LeleBase(json_str) {

    _class_name = __func__ ;//
  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "text") {
        _text = value;
      }
      else if(key == "max_length") {
        _max_length = value.empty() ? 15 : std::atoi(value.c_str());
      }
      else if(key == "multiline") {
        _multiline = value.empty() ? true : (std::atoi(value.c_str()) != 0);
      }
    }
  }
}

lv_obj_t *LeleTextbox::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {

  _lv_obj = LeleBase::createLvObj(lele_parent,
    lv_textarea_create(lele_parent->getLvObj()));

  lv_textarea_set_text(_lv_obj, _text.c_str());
  // lv_obj_align(_lv_obj, LV_ALIGN_TOP_MID, x, y);
  lv_textarea_set_text_selection(_lv_obj, true);
  lv_textarea_set_max_length(_lv_obj, _max_length);
  lv_textarea_set_one_line(_lv_obj, _multiline);
  lv_obj_add_event_cb(_lv_obj, LeleBase::EventCallback, LV_EVENT_CLICKED, this);//also triggered when Enter key is pressed

  return _lv_obj;
}

bool LeleTextbox::eventCallback(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED 
      && lv_event_get_target(e)==_lv_obj){
        const char * text = lv_textarea_get_text(_lv_obj);
        LOG(DEBUG, LVSIM, "Textarea Clicked: %s\n", text);
    }
    return true;
}
