#pragma once

#include "lelebase.h"

class LeleGroup : public LeleBase {
  public:
  LeleGroup(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  virtual bool eventCallback(LeleEvent &&e) override;
  protected:
  int _active_child_idx = -1;
};
class LeleButtons : public LeleBase {
  public:
  class LeleButton : public LeleBase  {
    public:
    enum Type {
      Push,
      Checkbox,
      Radio,
      Switch,
      Close
    };
    LeleButton(const std::string &json_str);
    virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
    virtual bool eventCallback(LeleEvent &&e) override;
    bool isCheckable() const { return _checkable; }
    bool isChecked() const { return _checked; }
    Type type() const { return _type; }
    const std::string &text() const { return _text; }
    protected:
    bool _checkable = false;
    bool _checked = false;
    Type _type = Push;
    std::string _text;
    std::vector<LeleEvent*> _events;
  };
  LeleButtons(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  int count() const;
  LeleButton* getAt(int idx) const;
  protected:
};


