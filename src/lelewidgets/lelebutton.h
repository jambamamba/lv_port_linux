#pragma once

#include "lelebase.h"

class LeleButton : public LeleBase  {
  public:
  LeleButton(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr) override;
  protected:
  static void EventCallback(lv_event_t * e);  
  std::string _text;
  bool _checkable = false;
};

