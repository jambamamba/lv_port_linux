#pragma once

#include "leleobject.h"

class LeleNullWidget : public LeleObject  {
  public:
  LeleNullWidget(const std::string &json_str = "")
  : LeleObject(json_str) {
    _class_name = __func__ ;//
  }
};
