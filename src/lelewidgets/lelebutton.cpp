#include "lelebutton.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleButton::LeleButton(const std::string &json_str)
  : LeleBase(json_str) {

  _id = __func__ ;//typeid(this).name();
  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "text") {
        _text = value;
      }
      else if(key == "checkable") {
        _checkable = strncmp(value.c_str(), "true", 4) == 0;
      }
    }
  }
}

lv_obj_t *LeleButton::createLvObj(LeleBase *lele_parent) {

  _lv_obj = lv_button_create(lele_parent->getLvObj());
  _lv_obj = LeleBase::createLvObj(lele_parent);

  lv_obj_add_event_cb(_lv_obj, EventCallback, LV_EVENT_ALL, this);
  // lv_obj_align(_lv_obj, LV_ALIGN_CENTER, 0, -40);
  lv_obj_remove_flag(_lv_obj, LV_OBJ_FLAG_PRESS_LOCK);

  lv_obj_t *label = lv_label_create(_lv_obj);
  lv_label_set_text(label, _text.c_str());
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
  if(_checkable) {
    lv_obj_add_flag(_lv_obj, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_style_bg_color(_lv_obj, lv_color_hex(_lele_style->checkedColor()), LV_PART_MAIN | LV_STATE_CHECKED); // Green when checked

  }

  return _lv_obj;
}


void LeleButton::EventCallback(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * lv_obj = ((LeleButton *)e->user_data)->getLvObj();

    if(code == LV_EVENT_CLICKED) {
        LOG(DEBUG, LVSIM, "Button Clicked\n");
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LOG(DEBUG, LVSIM, "Toggled\n");
    }
}
