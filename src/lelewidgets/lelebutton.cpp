#include "lelebutton.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleGroup::LeleGroup(const std::string &json_str)
  : LeleBase(json_str) {
    _class_name = __func__ ;//
}
lv_obj_t *LeleGroup::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {
  _lv_obj = LeleBase::createLvObj(lele_parent);
  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::unique_ptr<LeleBase>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleBase>>(token);
      value->createLvObj(this);
    }
  }
  lv_obj_add_event_cb(_lv_obj, LeleBase::EventCallback, LV_EVENT_CLICKED, this);//also triggered when Enter key is pressed

  return _lv_obj;
}
void LeleGroup::eventCallback(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    LeleBase *base = static_cast<LeleBase*>(e->user_data);
    LOG(DEBUG, LVSIM, "%s: clicked\n", base->getClassName().c_str());
    
    //uncheck all other buttons in the group, only one button should be checked at a time
    lv_obj_t *container = (lv_obj_t *)lv_event_get_current_target(e);//get the object to which an event was sent. I.e. the object whose event handler is being called.
    lv_obj_t *act_cb = lv_event_get_target_obj(e);//Get the object originally targeted by the event. It's the same even if the event is bubbled. 
    if(act_cb == container) {
      return;//Do nothing if the container was clicked
    }
    if(_active_child_idx > -1) {
      lv_obj_t *old_cb = lv_obj_get_child(container, _active_child_idx);
      lv_obj_remove_state(old_cb, LV_STATE_CHECKED);
    }
    lv_obj_add_state(act_cb, LV_STATE_CHECKED);
    _active_child_idx = lv_obj_get_index(act_cb);
}

LeleButtons::LeleButtons(const std::string &json_str)
  : LeleBase(json_str) {
    _class_name = __func__ ;//
}
lv_obj_t *LeleButtons::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {
  setParent(lele_parent);
  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::unique_ptr<LeleBase>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleBase>>(token);
      value->createLvObj(lele_parent);
    }
  }
  return _lv_obj;
}
int LeleButtons::count() const {
    int idx = 0;
    for(const auto &pair: _tokens) {
      if (std::holds_alternative<std::unique_ptr<LeleBase>>(pair.second)) {
        auto &value = std::get<std::unique_ptr<LeleBase>>(pair.second);
        if(pair.first == "button") {
          LeleButtons::LeleButton *button = dynamic_cast<LeleButtons::LeleButton*> (value.get());
          if(button) {
            ++idx;  
          }        
        }
      }
    }
    return idx;
}
LeleButtons::LeleButton* LeleButtons::getAt(int index) const {
    int idx = 0;
    for(const auto &pair: _tokens) {
      if (std::holds_alternative<std::unique_ptr<LeleBase>>(pair.second)) {
        auto &value = std::get<std::unique_ptr<LeleBase>>(pair.second);
        if(pair.first == "button") {
          LeleButtons::LeleButton *button = dynamic_cast<LeleButtons::LeleButton*> (value.get());
          if(button) {
            if(index == idx) {
              return button;
            }
            ++idx;
          }
        }
        // LOG(DEBUG, LVSIM, "Tabs token %s:%s\n", pair.first.c_str(), typeid(pair.second).name());
      }
    }
    return nullptr;
}

LeleButtons::LeleButton::LeleButton(const std::string &json_str)
  : LeleBase(json_str) {

  _class_name = __func__ ;//typeid(this).name();
  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "text") {
        _text = value;
      }
      if(key == "type") {
        if(value == "checkbox") {
          _type = LeleButtons::LeleButton::Type::Checkbox;
        }
        else if(value == "radio") {
          _type = LeleButtons::LeleButton::Type::Radio;
        }
        else if(value == "switch") {
          _type = LeleButtons::LeleButton::Type::Switch;
        }
        else {
          _type = LeleButtons::LeleButton::Type::Push;
        }
      }
      else if(key == "checkable") {
        _checkable = strncmp(value.c_str(), "true", 4) == 0;
      }
    }
  }
}

lv_obj_t *LeleButtons::LeleButton::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {

  switch(_type) {
    case LeleButtons::LeleButton::Type::Checkbox:
      _lv_obj = LeleBase::createLvObj(lele_parent, 
        lv_checkbox_create(lele_parent->getLvObj()));
      lv_checkbox_set_text(_lv_obj, _text.c_str());
    break;
    case LeleButtons::LeleButton::Type::Radio:
      _lv_obj = LeleBase::createLvObj(lele_parent, 
        lv_checkbox_create(lele_parent->getLvObj()));
      lv_style_set_radius(&_style, LV_RADIUS_CIRCLE);
      lv_obj_add_style(_lv_obj, &_style, LV_PART_INDICATOR);
      lv_checkbox_set_text(_lv_obj, _text.c_str());
    break;
    case LeleButtons::LeleButton::Type::Switch:
      _lv_obj = LeleBase::createLvObj(lele_parent, 
        lv_switch_create(lele_parent->getLvObj()));
      lv_obj_add_style(_lv_obj, &_style, LV_PART_INDICATOR);
    break;
    case LeleButtons::LeleButton::Type::Push:
    default:
      _lv_obj = LeleBase::createLvObj(lele_parent, 
        lv_button_create(lele_parent->getLvObj()));
      lv_obj_t *label = lv_label_create(_lv_obj);
      lv_label_set_text(label, _text.c_str());
      lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    break;
  }

  lv_obj_add_event_cb(_lv_obj, EventCallback, LV_EVENT_ALL, this);
  // lv_obj_align(_lv_obj, LV_ALIGN_CENTER, 0, -40);
  lv_obj_remove_flag(_lv_obj, LV_OBJ_FLAG_PRESS_LOCK);

  if(_checkable) {
    lv_obj_add_flag(_lv_obj, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_style_bg_color(_lv_obj, lv_color_hex(_lele_style->checkedColor()), LV_PART_MAIN | LV_STATE_CHECKED); // Green when checked
  }

  if(_lele_parent->getClassName() == "LeleGroup") { //bubble events to the parent if parent is a group
    lv_obj_add_flag(_lv_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
  }
  lv_obj_add_event_cb(_lv_obj, LeleBase::EventCallback, LV_EVENT_CLICKED, this);//also triggered when Enter key is pressed

  return _lv_obj;
}

void LeleButtons::LeleButton::eventCallback(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    // LeleGroup *group = dynamic_cast<LeleGroup>(_lele_parent);
    // if(group) {
    //   group->eventCallback(e);
    // }

    if(code == LV_EVENT_CLICKED) {
        LOG(DEBUG, LVSIM, "%s: clicked\n", _class_name.c_str());
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LOG(DEBUG, LVSIM, "%s: value changed\n", _class_name.c_str());
    }
}
