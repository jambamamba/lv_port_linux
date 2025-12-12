#include "leleevent.h"

#include "leleobject.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleEvent::LeleEvent(const std::string &json_str) {

  _class_name = __func__ ;//typeid(this).name();
  fromJson(json_str);
}
bool LeleEvent::fromJson(const std::string &json_str) {
  for (const auto &[key, token]: LeleWidgetFactory::fromJson(json_str)) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "id") {
        _id = value;
      }
      else if(key == "type") {
        _type = value;
      }
      else if(key == "action") {
        _action = value;
      }
      else if(key == "args") {
        parseArgs(value);
      }
    }
  }
  return true;
}

LeleEvent::LeleEvent(const LeleEvent& rhs, const lv_event_t *lv_event, int ivalue) 
: _id(rhs._id)
, _type(rhs._type)
, _action(rhs._action)
, _args(rhs._args)
, _lv_event(lv_event)
, _target_obj(rhs._target_obj)
, _code(lv_event ? lv_event_get_code(const_cast<lv_event_t*>(lv_event)) : 0)
, _ivalue(ivalue) {
}

void LeleEvent::parseArgs(const std::string &json_str) {
  for (const auto &[key, token]: LeleWidgetFactory::fromJson(json_str)) {
      if (std::holds_alternative<std::string>(token)) {
          const std::string &value = std::get<std::string>(token);
          _args[key] = value;
      }
  }
}

