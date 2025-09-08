#pragma once

#include "lelebase.h"

class LeleGroup : public LeleBase {
  public:
  LeleGroup(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr) override;
  virtual void eventCallback(lv_event_t * e) override;
};
class LeleButtons : public LeleBase {
  public:
  class LeleButton : public LeleBase  {
    public:
    LeleButton(const std::string &json_str);
    virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr) override;
    virtual void eventCallback(lv_event_t * e) override;
    protected:
    std::string _text;
    bool _checkable = false;
  };
  LeleButtons(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr) override;
  int count() const;
  LeleButton* getAt(int idx) const;
  protected:
};


