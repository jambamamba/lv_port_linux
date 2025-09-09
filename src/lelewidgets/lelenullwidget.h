#pragma once

#include "lelebase.h"

class LeleNullWidget : public LeleBase  {
  public:
  LeleNullWidget(const std::string &json_str = "")
  : LeleBase(json_str) {
    _class_name = __func__ ;//
  }
};
