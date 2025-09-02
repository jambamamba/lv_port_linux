#pragma once

#include "lelebase.h"

class LeleNullWidget : public LeleBase  {
  public:
  LeleNullWidget(const std::string &json_str = "", lv_obj_t *parent = lv_screen_active())
  : LeleBase(json_str, parent) {}
};
