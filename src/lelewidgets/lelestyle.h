#pragma once


#include <json_utils/json_utils.h>
#include <lvgl/lvgl_private.h>
#include <optional>
#include <string>

class LeleBase;
class LeleStyle {
  public:
  enum BorderTypeE {
    None=-1,Solid,Dashed,Dotted
  };
  // static LeleStyle fromJson(int parent_width = 0, int parent_height = 0, const cJSON *json = nullptr);
  // LeleStyle(int parent_width = 0, int parent_height = 0, const std::string &x = "", const std::string &y = "", const std::string &width = "", const std::string &height = "");
  LeleStyle(const std::string &json_str = "", lv_obj_t *parent = lv_screen_active());
  void setLeleParent(LeleBase *lele_parent) { _lele_parent = lele_parent; }
  static int parseColorCode(const std::string &color_str);
  static std::tuple<LeleStyle::BorderTypeE,int,int> parseBorder(const std::string &border_type_width_color);
  int x() const;
  int y() const;
  int width() const;
  int height() const;
  int cornerRadius() const;
  int padLeft() const;
  int padVer() const;
  int bgColor() const;
  int fgColor() const;
  int checkedColor() const;
  BorderTypeE borderType() const;
  int borderColor() const;
  int borderWidth() const;
  std::optional<lv_flex_flow_t> flow() const;
  protected:
  LeleBase *_lele_parent = nullptr;
  std::string _x;
  std::string _y;
  std::string _width;
  std::string _height;
  std::string _corner_radius;
  std::string _pad_left;
  std::string _pad_ver;
  std::string _border;
  std::string _flow;
  int _fgcolor = -1;//0  
  int _bgcolor = -1;//0xff0000;
  int _border_color = -1;
  int _border_width = -1;
  BorderTypeE _border_type = BorderTypeE::None;
  int _checked_color = -1;
  int _parent_width = 0;
  int _parent_height = 0;
};
