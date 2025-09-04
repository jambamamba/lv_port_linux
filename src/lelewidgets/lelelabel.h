#pragma once

#include "lelebase.h"

class LeleLabel : public LeleBase  {
  public:
  LeleLabel(const std::string &json_str);
  virtual lv_obj_t *createLvObj(lv_obj_t *lv_parent = lv_screen_active(), LeleBase *lele_parent = nullptr) override;
  protected:
  std::string _text;
};

