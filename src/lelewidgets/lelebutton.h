#pragma once

#include "leleobject.h"

class LeleGroup : public LeleObject {
  public:
  LeleGroup(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  virtual bool eventCallback(LeleEvent &&e) override;
  protected:
  int _active_child_idx = -1;
};
class LeleButtons : public LeleObject {
  public:
  class LeleButton : public LeleObject  {
    public:
    enum Type {
      Push,
      Checkbox,
      Radio,
      Switch,
      Close,
      Slider
    };
    LeleButton(const std::string &json_str);
    virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
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
    int _value = 0;
    std::vector<LeleEvent*> _events;
  };
  LeleButtons(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  int count() const;
  LeleButton* getAt(int idx) const;
  protected:
};


