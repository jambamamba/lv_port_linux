
#include <algorithm>
#include <regex>

#include "lelestyle.h"
#include "lelewidgetfactory.h"
#include "lelebase.h"

LOG_CATEGORY(LVSIM, "LVSIM");

namespace {
int absFromPercent(int percent, int parent) {
    return percent * parent / 100;
}
int toInt(const std::string &x, int parent_x) {
    if(x.size() > 0 && x.c_str()[x.size() - 1] == '%' && parent_x > 0) {
        return absFromPercent(std::atoi(x.c_str()), parent_x);
    }
    else if(x.size() > 0) {
        return std::atoi(x.c_str());
    }
    return 0;
}
}//namespace

LeleStyle::LeleStyle(const std::string &json_str, lv_obj_t *parent) 
  : _parent_width(lv_obj_get_width(parent))
  , _parent_height(lv_obj_get_height(parent)) {

  for (const auto &[key, token]: LeleWidgetFactory::fromJson(json_str)) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "x") {
        _x = value;
      }
      else if(key == "y") {
        _y = value;
      }
      else if(key == "width") {
        _width = value;
      }
      else if(key == "height") {
        _height = value;
      }
      else if(key == "corner_radius") {
        _corner_radius = value;
      }
      else if(key == "pad_ver") {
        _pad_ver = value;
      }
      else if(key == "pad_left") {
        _pad_left = value;
      }
      else if(key == "border") {
        std::tie(_border_type, _border_width, _border_color) = LeleStyle::parseBorder(value);
      }
      else if(key == "flow") {
        _flow = value;
      }
      else if(key == "fgcolor") {
        _fgcolor = LeleStyle::parseColorCode(value);
      }
      else if(key == "bgcolor") {
        _bgcolor = LeleStyle::parseColorCode(value);
      }
      else if(key == "checked_color") {
        _checked_color = LeleStyle::parseColorCode(value);
      }
    }
  }
}
int LeleStyle::parseColorCode(const std::string &color_str) {
  if(color_str.empty()) {
    return 0;
  }
  else if(strcmp(color_str.c_str(), "red") == 0) {
    return 0xff0000;
  }
  else if(strcmp(color_str.c_str(), "green") == 0) {
    return 0x00ff00;
  }
  else if(strcmp(color_str.c_str(), "blue") == 0) {
    return 0x0000ff;
  }
  else if(strcmp(color_str.c_str(), "white") == 0) {
    return 0xffffff;
  }
  else if(strcmp(color_str.c_str(), "black") == 0) {
    return 0x000000;
  }
  else if(std::all_of(color_str.begin(), color_str.end(),
    [](unsigned char ch){ return std::isdigit(ch); })) {
    return std::stoi(color_str.c_str(), nullptr, 10);
  }
  else if(color_str.c_str()[0] == '#') {
    std::string suffix(color_str.c_str() + 1);
    if(std::all_of(suffix.begin(), suffix.end(),
      [](unsigned char ch){return ch >= '0' && ch <= '0' + 0xff; })) {
        return std::stoi(suffix.c_str(), nullptr, 16);
    }
  }
  return 0;
}
std::tuple<LeleStyle::BorderTypeE,int,int> LeleStyle::parseBorder(const std::string &border_type_width_color) {

  LeleStyle::BorderTypeE border_type = LeleStyle::BorderTypeE::None;
  int border_width = 0;
  int border_color = 0;
  if(border_type_width_color.empty()) {
    return std::tuple<LeleStyle::BorderTypeE,int,int>{border_type, border_width, border_color};
  }
  std::regex pattern("(solid|none)-(\\d*)px-(#[a-f0-9]{1,6})");
  std::smatch matches;
  // std::string text("none-0px-#fcfcfc");
  // std::string text("solid-9px-#fcfcfc");
  if (std::regex_search(border_type_width_color, matches, pattern) &&
    matches.size() == 4) {
    if(matches[1] == "none") {
      border_type = None;
    }
    else if(matches[1] == "solid") {
      border_type = Solid;
    }
    else if(matches[1] == "dashed") {
      border_type = Dashed;
    }
    else if(matches[1] == "dotted") {
      border_type = Dotted;
    }
    border_width = std::atoi(matches[2].str().c_str());
    border_color = parseColorCode(matches[3]);
  }
  return std::tuple<LeleStyle::BorderTypeE,int,int>{border_type, border_width, border_color};
}
int LeleStyle::x() const {
  if(_x.empty()) {
    if(_lele_parent) {
      return _lele_parent->style()->x();
    }
    return 0;
  }
  return toInt(_x, _parent_width);
}
int LeleStyle::y() const {
  if(_y.empty()) {
    if(_lele_parent) {
      return _lele_parent->style()->y();
    }
    return 0;
  }
  return toInt(_y, _parent_height);
}
int LeleStyle::width() const {
  if(_width.empty()) {
    if(_lele_parent) {
      return _lele_parent->style()->width();
    }
    return 0;
  }
  return toInt(_width, _parent_width);
}
int LeleStyle::height() const {
  if(_height.empty()) {
    if(_lele_parent) {
      return _lele_parent->style()->height();
    }
    return 0;
  }
  return toInt(_height, _parent_height);
}
int LeleStyle::cornerRadius() const {
  if(_corner_radius.empty()) {
    if(_lele_parent) {
      return _lele_parent->style()->cornerRadius();
    }
    return 5;
  }
  return toInt(_corner_radius, std::max(_parent_height, _parent_width));  
}
int LeleStyle::padLeft() const {
  if(_pad_left.empty()) {
    if(_lele_parent) {
      return _lele_parent->style()->padLeft();
    }
    return 5;
  }
  return toInt(_pad_left, _parent_width);  
}
int LeleStyle::padVer() const {
  if(_pad_ver.empty()) {
    if(_lele_parent) {
      return _lele_parent->style()->padVer();
    }
    return 20;
  }
  return toInt(_pad_ver, _parent_height);
}
LeleStyle::BorderTypeE LeleStyle::borderType() const {
  if(_border_type == LeleStyle::BorderTypeE::None) {
    if(_lele_parent) {
      return _lele_parent->style()->borderType();
    }
    return LeleStyle::BorderTypeE::None;
  }
  return _border_type;
}
int LeleStyle::borderWidth() const {
  if(_border_width == -1) {
    if(_lele_parent) {
      return _lele_parent->style()->borderWidth();
    }
    return 0;
  }
  return _border_width;
}
int LeleStyle::borderColor() const {
  if(_border_color == -1) {
    if(_lele_parent) {
      return _lele_parent->style()->borderColor();
    }
    return 0;
  }
  return _border_color;
}
int LeleStyle::bgColor() const {
  if(_bgcolor == -1) {
    if(_lele_parent) {
      return _lele_parent->style()->bgColor();
    }
    return 0;
  }
  return _bgcolor;
}
int LeleStyle::fgColor() const {
  
  if(_fgcolor == -1) {
    if(_lele_parent) {
      int fgcolor = _lele_parent->style()->fgColor();
      // LOG(DEBUG, LVSIM, "parent:%s, fgcolor: 0x%x\n", _lele_parent->getClassName().c_str(), fgcolor);//osm todo: get color from parent class
      return fgcolor;
    }
  }
  return _fgcolor;
}
int LeleStyle::checkedColor() const {
  if(_checked_color == -1) {
    if(_lele_parent) {
      return _lele_parent->style()->checkedColor();
    }
    return 0;
  }
  return _checked_color;
}
std::optional<lv_flex_flow_t> LeleStyle::flow() const {
  
  if(strncmp(_flow.c_str(), "row", 6)==0) {
    return LV_FLEX_FLOW_ROW;
  }
  else if(strncmp(_flow.c_str(), "row_wrap", 8)==0) {
    return LV_FLEX_FLOW_ROW_WRAP;
  }
  else if(strncmp(_flow.c_str(), "row_reverse", 11)==0){
    return LV_FLEX_FLOW_ROW_REVERSE;
  }
  else if(strncmp(_flow.c_str(), "row_wrap_reverse", 16)==0){
    return LV_FLEX_FLOW_ROW_WRAP_REVERSE;
  }
  else if(strncmp(_flow.c_str(), "column", 6)==0) {
    return LV_FLEX_FLOW_COLUMN;//lv_obj_set_flex_flow(cont1, LV_FLEX_FLOW_COLUMN);
  }
  else if(strncmp(_flow.c_str(), "column_wrap", 11)==0){
    return LV_FLEX_FLOW_COLUMN_WRAP;
  }
  else if(strncmp(_flow.c_str(), "column_reverse", 14)==0){
    return LV_FLEX_FLOW_COLUMN_REVERSE;
  }
  else if(strncmp(_flow.c_str(), "column_wrap_reverse", 19)==0){
    return LV_FLEX_FLOW_COLUMN_WRAP_REVERSE;
  }
  return std::nullopt;
}