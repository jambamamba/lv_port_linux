#pragma once


#include <json_utils/json_utils.h>
#include <lvgl/lvgl_private.h>
#include <optional>
#include <string>
#include <variant>



class LeleBase;
class LeleStyle {
  public:
  enum BorderTypeE {
    None=-1,Solid,Dashed,Dotted
  };
  using StyleValue = std::variant<
    int,
    std::string,
    lv_flex_flow_t,
    BorderTypeE
  >;
  // static LeleStyle fromJson(int parent_width = 0, int parent_height = 0, const cJSON *json = nullptr);
  // LeleStyle(int parent_width = 0, int parent_height = 0, const std::string &x = "", const std::string &y = "", const std::string &width = "", const std::string &height = "");
  LeleStyle(const std::string &json_str = "", lv_obj_t *parent = lv_screen_active());
  friend std::ostream& operator<<(std::ostream& os, const LeleStyle& p);
  void setLeleParent(LeleBase *lele_parent) { _lele_parent = lele_parent; }
  LeleBase *getLeleParent() const { return _lele_parent; }
  static int parseColorCode(const std::string &color_str);
  static std::tuple<int,int,int,int> parsePaddingOrMargin(const std::string &padding_str);
  static std::tuple<LeleStyle::BorderTypeE,int,int> parseBorder(const std::string &border_type_width_color);
  static std::tuple<std::string,std::string,std::string,std::string> parseTopRightBottomLeft(const std::string &value);
  std::string className() const;
  std::optional<StyleValue> getValue(const std::string &key, std::string class_name = "") const;
  protected:
  LeleBase *_lele_parent = nullptr;
  std::string _class_name;
  std::string _id;
  std::map<std::string, std::optional<StyleValue>> _style = {
    {"x", std::nullopt},
    {"y", std::nullopt},
    {"width", std::nullopt},
    {"height", std::nullopt},
    {"corner_radius", std::nullopt},
    {"padding/top", std::nullopt},
    {"padding/right", std::nullopt},
    {"padding/bottom", std::nullopt},
    {"padding/left", std::nullopt},
    {"margin/top", std::nullopt},
    {"margin/right", std::nullopt},
    {"margin/bottom", std::nullopt},
    {"margin/left", std::nullopt},
    {"border_type", std::nullopt},
    {"border_width", std::nullopt},
    {"border_color", std::nullopt},
    {"flow", std::nullopt},
    {"grow", std::nullopt},
    {"fgcolor", std::nullopt},
    {"bgcolor", std::nullopt},
    {"checked_color", std::nullopt}
  };
  int _parent_width = 0;
  int _parent_height = 0;
};
class LeleStyles {
  public:
  LeleStyles(const std::string &json_str = "");
  friend std::ostream& operator<<(std::ostream& os, const LeleStyles& p);
  void setLeleParent(LeleBase *lele_parent);
  void addStyle(LeleStyle* lele_style);
  size_t size() const { return _lele_styles.size(); }
  LeleStyles &operator+=(LeleStyles &);
  LeleStyles &operator+=(LeleStyle &);
  std::optional<LeleStyle::StyleValue> getValue(const std::string &key, std::string class_name = "") const;
  protected:
  std::string _id;
  LeleStyle _null_pos;
  std::vector<LeleStyle *>_lele_styles;
  LeleBase *_lele_parent = nullptr;
};

