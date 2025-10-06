#pragma once

#include <string>
#include <map>
#include <src/lelewidgets/lelewidgetfactory.h>

// struct lv_event_t;
class LeleEvent {
  public:
  LeleEvent(const std::string &json_str);
  LeleEvent(lv_event_t *e) : _lv_event(e) {}
  LeleEvent(const LeleEvent& rhs, const lv_event_t *e);
  void copy(const std::string &id, const std::string &type, const std::string &action, const std::map<std::string, std::string> &args);
  const std::string &action() const { return _action; }
  const std::string &type() const { return _type; }
  const std::string &id() const { return _id; }
  const lv_event_t *lv_event() const { return _lv_event; }
  const std::map<std::string, std::string> &args() const { return _args; }
  protected:
  void parseArgs(const std::string &json_str);

  std::string _class_name;
  std::string _id;
  std::string _type;
  std::string _action;
  std::map<std::string, std::string> _args;
  const lv_event_t *_lv_event = nullptr;
};

