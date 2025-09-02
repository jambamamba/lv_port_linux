#pragma once

#include "lelebase.h"

class LelePos : public LeleBase {
  public:
  // static LelePos fromJson(int parent_width = 0, int parent_height = 0, const cJSON *json = nullptr);
  // LelePos(int parent_width = 0, int parent_height = 0, const std::string &x = "", const std::string &y = "", const std::string &width = "", const std::string &height = "");
  LelePos(const std::string &json_str = "", lv_obj_t *parent = lv_screen_active());
  int x() const;
  int y() const;
  int width() const;
  int height() const;
  protected:
  int absFromPercent(int percent, int parent) const;
  std::string _x = "0";
  std::string _y = "0";
  std::string _width = "0";
  std::string _height = "0";
  int _parent_width = 0;
  int _parent_height = 0;
};
