#include "lelebutton.h"

#include "leleevent.h"
#include "leleview.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleButtons::LeleButtons(LeleObject *parent, const std::string &json_str)
  : LeleObject(parent, json_str) {
  _class_name = __func__ ;//
  fromJson(json_str);
}
bool LeleButtons::LeleButtons::fromJson(const std::string &json_str) {
  return true;
}
lv_obj_t *LeleButtons::createLvObj(LeleObject *lele_parent, lv_obj_t *lv_obj) {
  setParent(lele_parent);
  for (const auto &[key, token]: _nodes) {
    if (std::holds_alternative<std::unique_ptr<LeleObject>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleObject>>(token);
      value->createLvObj(lele_parent);
    }
  }
  return _lv_obj;
}
int LeleButtons::count() const {
    int idx = 0;
    for(const auto &pair: _nodes) {
      if (std::holds_alternative<std::unique_ptr<LeleObject>>(pair.second)) {
        auto &value = std::get<std::unique_ptr<LeleObject>>(pair.second);
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
    for(const auto &pair: _nodes) {
      if (std::holds_alternative<std::unique_ptr<LeleObject>>(pair.second)) {
        auto &value = std::get<std::unique_ptr<LeleObject>>(pair.second);
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

LeleButtons::LeleButton::LeleButton(LeleObject *parent, const std::string &json_str)
  : LeleLabel(parent, json_str) {

  _class_name = __func__ ;//typeid(this).name();
  fromJson(json_str);
}
bool LeleButtons::LeleButton::fromJson(const std::string &json_str) {
  for (const auto &[key, token]: _nodes) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "text") {
        _text = value;
      }
      else if(key == "value") {
        _value = std::stoi(value);
      }
      else if(key == "start") {
        _start_value = std::stoi(value);
      }
      else if(key == "end") {
        _end_value = std::stoi(value);
      }
      else if(key == "min") {
        _min = std::stoi(value);
      }
      else if(key == "max") {
        _max = std::stoi(value);
      }
      else if(key == "rotation") {
        _rotation = std::stoi(value);
      }
      else if(key == "type") {
        if(value == "checkbox") {
          _type = LeleButtons::LeleButton::Type::Checkbox;
        }
        else if(value == "radio") {
          _type = LeleButtons::LeleButton::Type::Radio;
        }
        else if(value == "switch") {
          _type = LeleButtons::LeleButton::Type::Switch;
        }
        else if(value == "slider") {
          _type = LeleButtons::LeleButton::Type::Slider;
        }
        else if(value == "slider-range") {
          _type = LeleButtons::LeleButton::Type::SliderRange;
        }
        else if(value == "color-picker") {
          _type = LeleButtons::LeleButton::Type::ColorPicker;
        }
        else if(value == "arc") {
          _type = LeleButtons::LeleButton::Type::Arc;
        }
        else if(value == "close") {
          _type = LeleButtons::LeleButton::Type::Close;
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
  return true;
}
lv_obj_t *LeleButtons::LeleButton::createLvObj(LeleObject *lele_parent, lv_obj_t *lv_obj) {

  switch(_type) {
    case LeleButtons::LeleButton::Type::Checkbox:{
      _lv_obj = LeleObject::createLvObj(lele_parent, 
        lv_obj ? lv_obj : lv_checkbox_create(lele_parent->getLvObj()));
      lv_obj_remove_style(_lv_obj, &_style, LV_PART_MAIN);
      lv_obj_add_style(_lv_obj, &_style, LV_PART_INDICATOR);
      lv_checkbox_set_text(_lv_obj, _text.c_str());
      break;
    }
    case LeleButtons::LeleButton::Type::Radio:{
      _lv_obj = LeleObject::createLvObj(lele_parent, 
        lv_obj ? lv_obj : lv_checkbox_create(lele_parent->getLvObj()));
      lv_obj_remove_style(_lv_obj, &_style, LV_PART_MAIN);
      lv_obj_add_style(_lv_obj, &_style, LV_PART_INDICATOR);
      lv_checkbox_set_text(_lv_obj, _text.c_str());
      break;
    }
    case LeleButtons::LeleButton::Type::Switch:{
      _lv_obj = LeleObject::createLvObj(lele_parent, 
        lv_obj ? lv_obj : lv_switch_create(lele_parent->getLvObj()));
      lv_obj_remove_style(_lv_obj, &_style, LV_PART_MAIN);
      lv_obj_add_style(_lv_obj, &_style, LV_PART_INDICATOR);
      break;
    }
    case LeleButtons::LeleButton::Type::SliderRange:
    case LeleButtons::LeleButton::Type::Slider:{
      _lv_obj = LeleObject::createLvObj(lele_parent, 
        lv_obj ? lv_obj : lv_slider_create(lele_parent->getLvObj()));
      lv_slider_set_range(_lv_obj, _min, _max);
      lv_slider_set_value(_lv_obj, _value, LV_ANIM_OFF);
      if(_type == LeleButtons::LeleButton::Type::SliderRange) {
        lv_slider_set_mode(_lv_obj, LV_SLIDER_MODE_RANGE);
        lv_slider_set_start_value(_lv_obj, _start_value, LV_ANIM_OFF);
      }
      break;
    }
    case LeleButtons::LeleButton::Type::Arc:{
      _lv_obj = LeleObject::createLvObj(lele_parent, 
        lv_obj ? lv_obj : lv_arc_create(lele_parent->getLvObj()));
        auto width = getStyle("width");
        auto height = getStyle("height");
        if(width && height) {
          lv_obj_set_size(_lv_obj, std::get<int>(width.value()), std::get<int>(height.value()));
        }
        lv_arc_set_range(_lv_obj, _min, _max);
        lv_arc_set_rotation(_lv_obj, _rotation);
        lv_arc_set_bg_angles(_lv_obj, _start_value, _end_value);
        lv_arc_set_value(_lv_obj, _value);
        lv_obj_center(_lv_obj);
      break;
    }
    case LeleButtons::LeleButton::Type::ColorPicker:{
      //use lv_indev_get_point(getWindow(_backend)->lv_indev_pointer, &point); to get point clicked, then get the color at that coordinate
      // _lv_obj = LeleObject::createLvObj(lele_parent, 
      //   lv_obj ? lv_obj : lv_cpicker_create(lele_parent->getLvObj()));//https://docs.lvgl.io/7.11/widgets/cpicker.html
      //   auto width = getStyle("width");
      //   auto height = getStyle("height");
      //   if(width && height) {
      //     lv_obj_set_size(_lv_obj, std::get<int>(width.value()), std::get<int>(height.value()));
      //   }
      //   // lv_arc_set_rEventCallbackange(_lv_obj, _min, _max);
      //   // lv_arc_set_value(_lv_obj, _value);//value: 0-100
      //   lv_cpicker_set_color(_lv_obj, _value);
      //   lv_obj_center(_lv_obj);
      break;
    }
    case LeleButtons::LeleButton::Type::Close: {
      _lv_obj = LeleObject::createLvObj(lele_parent, 
        lv_obj ? lv_obj : lv_button_create(lele_parent->getLvObj()));
      lv_obj_t *img = lv_image_create(_lv_obj);
      lv_image_set_src(img, LV_SYMBOL_CLOSE);
      lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
      break;
    }
    case LeleButtons::LeleButton::Type::Push:
    default: {
      _lv_obj = LeleObject::createLvObj(lele_parent, 
        lv_obj ? lv_obj : lv_button_create(lele_parent->getLvObj()));
      lv_obj_t *label = lv_label_create(_lv_obj);
      lv_label_set_text(label, _text.c_str());
      // lv_obj_set_layout(_lv_obj, LV_LAYOUT_NONE);
      // lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
      setObjAlignStyle(label);//osm todo: not working, cannot center text
      break;
    }
  }

  lv_obj_remove_flag(_lv_obj, LV_OBJ_FLAG_PRESS_LOCK);

  if(_checkable) {
    lv_obj_add_flag(_lv_obj, LV_OBJ_FLAG_CHECKABLE);
    auto value = getStyle("checked_color");
    if(value) {
      lv_obj_set_style_bg_color(_lv_obj, lv_color_hex(std::get<int>(value.value())), (int)LV_PART_MAIN | (int)LV_STATE_CHECKED); // Green when checked
    }
  }
  setChecked(_checked);

  const LeleView *view = dynamic_cast<const LeleView*>(_lele_parent);
  if(view && view->isGroup()) {
    lv_obj_add_flag(_lv_obj, LV_OBJ_FLAG_EVENT_BUBBLE);//bubble events to the parent if parent is a group
  }

  return _lv_obj;
}

void LeleButtons::LeleButton::setChecked(bool checked) { 
  _checked = checked; 
  if(_checked) {
    lv_obj_add_state(_lv_obj, LV_STATE_CHECKED);
  }
  else {
    lv_obj_remove_state(_lv_obj, LV_STATE_CHECKED);
  }
}

bool LeleButtons::LeleButton::click() {
  return LV_RESULT_OK == lv_obj_send_event(_lv_obj, LV_EVENT_CLICKED, nullptr);
}

void LeleButtons::LeleButton::setValue(int value) {
  _value = value;
  if(_type == LeleButtons::LeleButton::Type::Slider ||
    _type == LeleButtons::LeleButton::Type::SliderRange) {
    lv_slider_set_value(_lv_obj, _value, LV_ANIM_OFF);
  }
  else if(_type == LeleButtons::LeleButton::Type::Arc) {
    lv_arc_set_value(_lv_obj, _value);
  }

  lv_event_t event = {
    .current_target=_lv_obj,
    .original_target=_lv_obj,
    .code=LV_EVENT_VALUE_CHANGED,
    .user_data=this
  };
  LeleObject::EventCallback(&event);
  // lv_result_t lv_obj_send_event(lv_obj_t * obj, lv_event_code_t event_code, void * param)
}

bool LeleButtons::LeleButton::eventCallback(LeleEvent &&e) {
    lv_event_t* lv_event = const_cast<lv_event_t*>(e.getLvEvent());
    lv_event_code_t code = lv_event_get_code(lv_event);
    // LeleView *view = dynamic_cast<LeleView>(_lele_parent);
    // if(view) {
    //   view->eventCallback(e);
    // }

    if(code == LV_EVENT_CLICKED) {
        // LOG(DEBUG, LVSIM, "%s: clicked. button type:%i\n", _class_name.c_str(), _type);
        for(LeleEvent *event: _events) {
          if(event->getType() == "clicked"){
            // e->copy(event.id(), event->type(), event->action(), event->args);
            // LOG(DEBUG, LVSIM, "LeleButtons::LeleButton::eventCallback\n");
            event->setTargetObj(e.getTargetObj());
            return LeleObject::eventCallback(LeleEvent(*event, lv_event));
          }
        }
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *lv_obj = lv_event_get_target_obj(lv_event);
        if(lv_obj->class_p == &lv_arc_class) {
          _value = lv_arc_get_value(lv_obj);
          LOG(DEBUG, LVSIM, "%s: value changed. button type:%i, value:%i\n", _class_name.c_str(), _type, _value);
          return LeleObject::eventCallback(LeleEvent(e, lv_event, _value));
        }
        else if(lv_obj->class_p == &lv_slider_class) {
          _value = lv_slider_get_value(lv_obj);
          if(_type == LeleButtons::LeleButton::Type::SliderRange) {
            _start_value = lv_slider_get_left_value(lv_obj);
          }
          LOG(DEBUG, LVSIM, "%s: value changed. button type:%i, value:%i, start_value:%i\n", _class_name.c_str(), _type, _value, _start_value);
          return LeleObject::eventCallback(LeleEvent(e, lv_event, _value, _start_value));
        }
        else {
          LL(WARNING, LVSIM) << "No handler for value changed event!";
        }
        return LeleObject::eventCallback(LeleEvent(e, lv_event, _value));
    }
    return LeleObject::eventCallback(LeleEvent(e, lv_event));
}
