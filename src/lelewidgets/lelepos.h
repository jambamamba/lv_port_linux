#pragma once

#include <debug_logger/debug_logger.h>
#include <json_utils/json_utils.h>
#include <lvgl/lvgl_private.h>
#include <memory>
#include <optional>
#include <res/img_dsc.h>
#include <string.h>
#include <string>
#include <vector>

class LelePos {
  public:
  static LelePos fromJson(int parent_width = 0, int parent_height = 0, const cJSON *json = nullptr);
  LelePos(int parent_width = 0, int parent_height = 0, const std::string &x = "", const std::string &y = "", const std::string &width = "", const std::string &height = "");
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
