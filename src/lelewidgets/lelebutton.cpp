#include "lelebutton.h"

#include "leleevent.h"
#include "leleview.h"

LOG_CATEGORY(LVSIM, "LVSIM");

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
      else if(key == "checked") {
        _checked = strncmp(value.c_str(), "true", 4) == 0;
      }
    }
    else if(std::holds_alternative<std::unique_ptr<LeleEvent>>(token)) {
      LeleEvent *event = std::get<std::unique_ptr<LeleEvent>>(token).get();
      _events.push_back(event);
    }
  }
}
lv_obj_t *LeleButtons::LeleButton::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {

  switch(_type) {
    case LeleButtons::LeleButton::Type::Checkbox:
      _lv_obj = LeleBase::createLvObj(lele_parent, 
        lv_checkbox_create(lele_parent->getLvObj()));
      lv_obj_remove_style(_lv_obj, &_style, LV_PART_MAIN);
      lv_obj_add_style(_lv_obj, &_style, LV_PART_INDICATOR);
      lv_checkbox_set_text(_lv_obj, _text.c_str());
    break;
    case LeleButtons::LeleButton::Type::Radio:
      _lv_obj = LeleBase::createLvObj(lele_parent, 
        lv_checkbox_create(lele_parent->getLvObj()));
      lv_obj_remove_style(_lv_obj, &_style, LV_PART_MAIN);
      lv_obj_add_style(_lv_obj, &_style, LV_PART_INDICATOR);
      lv_checkbox_set_text(_lv_obj, _text.c_str());
    break;
    case LeleButtons::LeleButton::Type::Switch:
      _lv_obj = LeleBase::createLvObj(lele_parent, 
        lv_switch_create(lele_parent->getLvObj()));
      lv_obj_remove_style(_lv_obj, &_style, LV_PART_MAIN);
      lv_obj_add_style(_lv_obj, &_style, LV_PART_INDICATOR);
    break;
    case LeleButtons::LeleButton::Type::Push:
    default:
      _lv_obj = LeleBase::createLvObj(lele_parent, 
        lv_button_create(lele_parent->getLvObj()));
      lv_obj_t *label = lv_label_create(_lv_obj);
      lv_label_set_text(label, _text.c_str());
      setObjAlignStyle(label);
      lv_obj_set_layout(_lv_obj, LV_LAYOUT_NONE);
    break;
  }

  lv_obj_add_event_cb(_lv_obj, EventCallback, LV_EVENT_ALL, this);
  lv_obj_remove_flag(_lv_obj, LV_OBJ_FLAG_PRESS_LOCK);

  if(_checkable) {
    lv_obj_add_flag(_lv_obj, LV_OBJ_FLAG_CHECKABLE);
    auto value =_lele_styles.getValue("checked_color");
    if(value) {
      lv_obj_set_style_bg_color(_lv_obj, lv_color_hex(std::get<int>(value.value())), LV_PART_MAIN | LV_STATE_CHECKED); // Green when checked
    }
  }
  if(_checked) {
    lv_obj_add_state(_lv_obj, LV_STATE_CHECKED);
  }
  else {
    lv_obj_remove_state(_lv_obj, LV_STATE_CHECKED);
  }

  LeleView *view = dynamic_cast<LeleView*>(_lele_parent);
  if(view && view->isGroup()) {
    lv_obj_add_flag(_lv_obj, LV_OBJ_FLAG_EVENT_BUBBLE);//bubble events to the parent if parent is a group
  }
  // lv_obj_add_event_cb(_lv_obj, LeleBase::EventCallback, LV_EVENT_CLICKED, this);//also triggered when Enter key is pressed

  return _lv_obj;
}

bool LeleButtons::LeleButton::eventCallback(LeleEvent &&e) {
    lv_event_t* lv_event = const_cast<lv_event_t*>(e.lv_event());
    lv_event_code_t code = lv_event_get_code(lv_event);
    // LeleView *view = dynamic_cast<LeleView>(_lele_parent);
    // if(view) {
    //   view->eventCallback(e);
    // }

    if(code == LV_EVENT_CLICKED) {
        // LOG(DEBUG, LVSIM, "%s: clicked. button type:%i\n", _class_name.c_str(), _type);
        for(LeleEvent *event: _events) {
          if(event->type() == "clicked"){
            // e->copy(event.id(), event->type(), event->action(), event->args);
            // LOG(DEBUG, LVSIM, "LeleButtons::LeleButton::eventCallback\n");
            return LeleBase::eventCallback(LeleEvent(*event, lv_event));
          }
        }
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LOG(DEBUG, LVSIM, "%s: value changed. button type:%i\n", _class_name.c_str(), _type);
    }
    return LeleBase::eventCallback(std::move(e));
}
