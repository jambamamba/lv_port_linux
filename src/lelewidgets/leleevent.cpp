#include "leleevent.h"

#include "lelebase.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleEvent::LeleEvent(const std::string &json_str) {

  _class_name = __func__ ;//typeid(this).name();
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
}

void LeleEvent::parseArgs(const std::string &json_str) {
  for (const auto &[key, token]: LeleWidgetFactory::fromJson(json_str)) {
      if (std::holds_alternative<std::string>(token)) {
          const std::string &value = std::get<std::string>(token);
          _args[key] = value;
      }
  }
}