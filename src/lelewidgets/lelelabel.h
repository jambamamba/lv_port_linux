#pragma once

#include "lelebase.h"

class LeleLabel : public LeleBase  {
  public:
  LeleLabel(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  void setText(const std::string &text);
  std::string getText() const;
  protected:
  std::string _text;
};

