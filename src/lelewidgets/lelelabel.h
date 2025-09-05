#pragma once

#include "lelebase.h"

class LeleLabel : public LeleBase  {
  public:
  LeleLabel(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr) override;
  protected:
  std::string _text;
};

