#pragma once

#include "lelebase.h"
#include "lelebutton.h"

class LeleEvent;
class LeleMessageBox : public LeleBase  {
  public:
  LeleMessageBox(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  virtual bool eventCallback(LeleEvent &&e) override;
  void setText(const std::string &text);
  std::string getText() const;
  protected:
  std::string _title;
  std::string _text;
  std::unique_ptr<LeleButtons::LeleButton> _btn;
  std::vector<LeleEvent*> _events;
};

