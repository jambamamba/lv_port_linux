
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
        int i = 0;
        if(x.size() > 2 && x.c_str()[0] == '0' && x.c_str()[1] == 'x') {
          i = std::stoi(x, 0, 16);
        }
        else {
          i = std::stoi(x, 0, 10);
        }
        return absFromPercent(i, parent_x);
    }
    else if(x.size() > 0) {
        int i = 0;
        if(x.size() > 2 && x.c_str()[0] == '0' && x.c_str()[1] == 'x') {
          i = std::stoi(x, 0, 16);
        }
        else {
          i = std::stoi(x, 0, 10);
        }
        return i;
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
      if(key == "class_name") {
        _class_name = value;
      }
      else if(key == "x") {
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
      else if(key == "padding") {
        _padding = value;
      }
      else if(key == "margin") {
        _margin = value;
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
std::tuple<int,int,int,int> LeleStyle::parsePaddingOrMargin(const std::string &padding_str) {

  int top = 0;
  int right = 0;
  int bottom = 0;
  int left = 0;
  if(padding_str.empty() || padding_str == "none") {
    return std::tuple<int,int,int,int>{top,right,bottom,left};
  }
  std::regex pattern("(\\d*)px\\s+(\\d*)px\\s+(\\d*)px\\s+(\\d*)px\\s*");
  std::smatch matches;
  // std::string text("0px 0px 0px 0px");
  if (std::regex_search(padding_str, matches, pattern) &&
    matches.size() == 5) {
    top = std::atoi(matches[1].str().c_str());
    right = std::atoi(matches[2].str().c_str());
    bottom = std::atoi(matches[3].str().c_str());
    left = std::atoi(matches[4].str().c_str());
  }
  return std::tuple<int,int,int,int>{top,right,bottom,left};
}

std::string LeleStyle::className() const {
  return _class_name;
}

int LeleStyle::x(std::string class_name) const {
  if(class_name.empty()) {
    class_name = _class_name;
  }
  if(!_x.empty() && class_name == _class_name) {
    return toInt(_x, _parent_width);
  }
  if(_lele_parent) {
    return _lele_parent->style()->x(class_name);
  }
  return 0;
}
int LeleStyle::y(std::string class_name) const {
  if(class_name.empty()) {
    class_name = _class_name;
  }
  if(!_y.empty() && class_name == _class_name) {
    return toInt(_y, _parent_height);
  }
  if(_lele_parent) {
    return _lele_parent->style()->y(class_name);
  }
  return 0;
}
int LeleStyle::width(std::string class_name) const {
  if(class_name.empty()) {
    class_name = _class_name;
  }
  if(!_width.empty() && class_name == _class_name) {
    return toInt(_width, _parent_width);
  }
  if(_lele_parent) {
    return _lele_parent->style()->width(class_name);
  }
  return 0;
}
int LeleStyle::height(std::string class_name) const {
  if(class_name.empty()) {
    class_name = _class_name;
  }
  if(!_height.empty() && class_name == _class_name) {
    return toInt(_height, _parent_height);
  }    
  if(_lele_parent) {
    return _lele_parent->style()->height(class_name);
  }
  return 0;
}
int LeleStyle::cornerRadius(std::string class_name) const {
  if(class_name.empty()) {
    class_name = _class_name;
  }
  if(!_corner_radius.empty() && class_name == _class_name) {
    return toInt(_corner_radius, std::max(_parent_height, _parent_width));  
  }
  if(_lele_parent) {
    return _lele_parent->style()->cornerRadius(class_name);
  }
  return 5;
}
std::tuple<int,int,int,int> LeleStyle::padding(std::string class_name) const {
  if(class_name.empty()) {
    class_name = _class_name;
  }
  if(!_padding.empty() && class_name == _class_name) {
    return parsePaddingOrMargin(_padding);
  }
  if(_lele_parent) {
    return _lele_parent->style()->padding(class_name);
  }
  return std::tuple<int,int,int,int>{0,0,0,0};
}
std::tuple<int,int,int,int> LeleStyle::margin(std::string class_name) const {
  if(class_name.empty()) {
    class_name = _class_name;
  }
  if(!_margin.empty() && class_name == _class_name) {
    return parsePaddingOrMargin(_margin);
  }
  if(_lele_parent) {
    return _lele_parent->style()->margin(class_name);
  }
  return std::tuple<int,int,int,int>{0,0,0,0};
}
LeleStyle::BorderTypeE LeleStyle::borderType(std::string class_name) const {
  if(class_name.empty()) {
    class_name = _class_name;
  }
  if(!_border_type != LeleStyle::BorderTypeE::None && class_name == _class_name) {
    return _border_type;
  }
  if(_lele_parent) {
    return _lele_parent->style()->borderType(class_name);
  }
  return LeleStyle::BorderTypeE::None;
}
int LeleStyle::borderWidth(std::string class_name) const {
  if(class_name.empty()) {
    class_name = _class_name;
  }
  if(_border_width != -1 && class_name == _class_name) {
    return _border_width;
  }
  if(_lele_parent) {
    return _lele_parent->style()->borderWidth(class_name);
  }
  return 0;
}
int LeleStyle::borderColor(std::string class_name) const {
  if(class_name.empty()) {
    class_name = _class_name;
  }
  if(_border_color != -1 && class_name == _class_name) {
    return _border_color;
  }
  if(_lele_parent) {
    return _lele_parent->style()->borderColor(class_name);
  }
  return 0;
}
int LeleStyle::bgColor(std::string class_name) const {
  if(class_name.empty()) {
    class_name = _class_name;
  }
  if(_bgcolor != -1 && class_name == _class_name) {
    return _bgcolor;
  }
  if(_lele_parent) {
    return _lele_parent->style()->bgColor(class_name);
  }
  return 0;
}
int LeleStyle::fgColor(std::string class_name) const {
  if(class_name.empty()) {
    class_name = _class_name;
  }
  if(_fgcolor != -1 && class_name == _class_name) {
    return _fgcolor;
  }
  if(_lele_parent) {
    int fgcolor = _lele_parent->style()->fgColor(class_name);
    // LOG(DEBUG, LVSIM, "parent:%s, fgcolor: 0x%x\n", _lele_parent->className().c_str(), fgcolor);//osm todo: get color from parent class
    return fgcolor;
  }
  return 0xffffff;
}
int LeleStyle::checkedColor(std::string class_name) const {
  if(class_name.empty()) {
    class_name = _class_name;
  }
  if(_checked_color != -1 && class_name == _class_name) {
    return _checked_color;
  }
  if(_lele_parent) {
    return _lele_parent->style()->checkedColor(class_name);
  }
  return 0;
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