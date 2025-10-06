#pragma once

#include <string>
#include <map>

class LeleEvent {
  public:
  LeleEvent(const std::string &json_str);
  const std::string &action() const { return _action; }
  const std::string &type() const { return _type; }
  const std::string &idtype() const { return _id; }
  const std::map<std::string, std::string> &args() const { return _args; }
  protected:
  void parseArgs(const std::string &json_str);

  std::string _class_name;
  std::string _id;
  std::string _type;
  std::string _action;
  std::map<std::string, std::string> _args;
};

