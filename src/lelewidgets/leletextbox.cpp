#include "leletextbox.h"


LOG_CATEGORY(LVSIM, "LVSIM");

LeleTextbox::LeleTextbox(const std::string &json_str)
  : LeleBase(json_str) {

    _class_name = __func__ ;//
  for (const auto &[key, token]: _nodes) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "text") {
        _text = value;
      }
      else if(key == "max_length") {
        constexpr int TEXTBOX_DEFAULT_MAX_LEN = 32;
        _max_length = value.empty() ? TEXTBOX_DEFAULT_MAX_LEN : std::stoi(value);
      }
      else if(key == "multiline") {
        _multiline = (value.empty() || value == "true");
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

bool LeleTextbox::eventCallback(LeleEvent &&e) {
    lv_event_t* lv_event = const_cast<lv_event_t*>(e.lv_event());
    lv_event_code_t code = lv_event_get_code(lv_event);

    if(code == LV_EVENT_CLICKED 
      && lv_event_get_target(lv_event)==_lv_obj){
        const char * text = lv_textarea_get_text(_lv_obj);
        LOG(DEBUG, LVSIM, "Textarea Clicked: %s\n", text);
    }
    return LeleBase::eventCallback(std::move(e));
}
