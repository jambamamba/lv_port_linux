#pragma once

#include "lelebase.h"

class LeleEvent : public LeleBase  {
  public:
  LeleEvent(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  protected:
  void parseArgs(const std::string &json_str);

  std::string _type;
  std::string _action;
  std::map<std::string, std::string> _args;
};

