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
  static std::tuple<int,int,int,int> parsePaddingOrMargin(const std::string &padding_str);
  static std::tuple<LeleStyle::BorderTypeE,int,int> parseBorder(const std::string &border_type_width_color);
  std::string className() const;
  std::optional<int> x(std::string class_name = "") const;
  std::optional<int> y(std::string class_name = "") const;
  std::optional<int> width(std::string class_name = "") const;
  std::optional<int> height(std::string class_name = "") const;
  std::optional<int> cornerRadius(std::string class_name = "") const;
  std::optional<std::tuple<int,int,int,int>> padding(std::string class_name = "") const;
  std::optional<std::tuple<int,int,int,int>> margin(std::string class_name = "") const;
  std::optional<int> bgColor(std::string class_name = "") const;
  std::optional<int> fgColor(std::string class_name = "") const;
  std::optional<int> checkedColor(std::string class_name = "") const;
  std::optional<BorderTypeE> borderType(std::string class_name = "") const;
  std::optional<int> borderColor(std::string class_name = "") const;
  std::optional<int> borderWidth(std::string class_name = "") const;
  std::optional<lv_flex_flow_t> flow(std::string class_name = "") const;
  protected:
  LeleBase *_lele_parent = nullptr;
  std::string _class_name;
  std::string _x;
  std::string _y;
  std::string _width;
  std::string _height;
  std::string _corner_radius;
  std::string _padding;
  std::string _margin;
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
class LeleStyles {
  public:
  void setLeleParent(LeleBase *lele_parent);
  void addStyle(LeleStyle* lele_style);
  int x(std::string class_name = "") const;
  int y(std::string class_name = "") const;
  int width(std::string class_name = "") const;
  int height(std::string class_name = "") const;
  int cornerRadius(std::string class_name = "") const;
  std::tuple<int,int,int,int> padding(std::string class_name = "") const;
  std::tuple<int,int,int,int> margin(std::string class_name = "") const;
  int bgColor(std::string class_name = "") const;
  int fgColor(std::string class_name = "") const;
  int checkedColor(std::string class_name = "") const;
  LeleStyle::BorderTypeE borderType(std::string class_name = "") const;
  int borderColor(std::string class_name = "") const;
  int borderWidth(std::string class_name = "") const;
  std::optional<lv_flex_flow_t> flow(std::string class_name = "") const;
  protected:
  LeleStyle _null_pos;
  std::vector<LeleStyle *>_lele_styles;
};