#pragma once


#include <json_utils/json_utils.h>
#include <lvgl/lvgl_private.h>
#include <string>

class LeleBase;
class LeleStyle {
  public:
  // static LeleStyle fromJson(int parent_width = 0, int parent_height = 0, const cJSON *json = nullptr);
  // LeleStyle(int parent_width = 0, int parent_height = 0, const std::string &x = "", const std::string &y = "", const std::string &width = "", const std::string &height = "");
  LeleStyle(const std::string &json_str = "", lv_obj_t *parent = lv_screen_active());
  void setLeleParent(LeleBase *lele_parent) { _lele_parent = lele_parent; }
  static int parseColorCode(const std::string &color_str);
  int x() const;
  int y() const;
  int width() const;
  int height() const;
  int cornerRadius() const;
  int padLeft() const;
  int padVer() const;
  int bgColor() const;
  int fgColor() const;
  int borderColor() const;
  int borderWidth() const;
  int checkedColor() const;
  protected:
  LeleBase *_lele_parent = nullptr;
  std::string _x = "0";
  std::string _y = "0";
  std::string _width = "0";
  std::string _height = "0";
  std::string _corner_radius = "5";
  std::string _pad_ver = "20";
  std::string _pad_left = "5";
  std::string _border_width = "0";
  std::string _flow = "row_wrapped";
  int _fgcolor = -1;//0  
  int _bgcolor = -1;//0xff0000;
  int _border_color = -1;
  int _checked_color = -1;
  int _parent_width = 0;
  int _parent_height = 0;
};
