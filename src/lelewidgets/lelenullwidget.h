#pragma once

#include "leleobject.h"

class LeleNullWidget : public LeleObject  {
  public:
  LeleNullWidget(const LeleObject *parent, const std::string &json_str = "")
  : LeleObject(parent, json_str) {
    _class_name = __func__ ;//
  }
};
