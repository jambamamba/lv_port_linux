#pragma once

#include "lelebase.h"

class LeleButtons : public LeleBase {
  public:
  class LeleButton : public LeleBase  {
    public:
    LeleButton(const std::string &json_str);
    virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr) override;
    protected:
    static void EventCallback(lv_event_t * e);  
    std::string _text;
    bool _checkable = false;
  };
  LeleButtons(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr) override;
  int count() const;
  LeleButtons::Button* getAt(int idx) const;
  protected:
};


