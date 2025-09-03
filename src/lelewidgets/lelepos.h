#pragma once


#include <json_utils/json_utils.h>
#include <lvgl/lvgl_private.h>
#include <string>

class LelePos {
  public:
  // static LelePos fromJson(int parent_width = 0, int parent_height = 0, const cJSON *json = nullptr);
  // LelePos(int parent_width = 0, int parent_height = 0, const std::string &x = "", const std::string &y = "", const std::string &width = "", const std::string &height = "");
  LelePos(const std::string &json_str = "", lv_obj_t *parent = lv_screen_active());
  int x() const;
  int y() const;
  int width() const;
  int height() const;
  int cornerRadius() const;
  int padLeft() const;
  int padVer() const;
  int bgColor() const;
  protected:
  std::string _x = "0";
  std::string _y = "0";
  std::string _width = "0";
  std::string _height = "0";
  std::string _corner_radius = "5";
  std::string _pad_ver = "20";
  std::string _pad_left = "5";
  int _fgcolor = 0;  
  int _bgcolor = 0xffffff;  
  int _parent_width = 0;
  int _parent_height = 0;
};
