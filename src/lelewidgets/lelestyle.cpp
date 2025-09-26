
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
      else if(key == "id") {
        _id = value;
      }
      else if(key == "x") {
        _style[key] = toInt(value, _parent_width);
      }
      else if(key == "y") {
        _style[key] = toInt(value, _parent_height);
      }
      else if(key == "width") {
        _style[key] = toInt(value, _parent_width);
      }
      else if(key == "height") {
        _style[key] = toInt(value, _parent_height);
      }
      else if(key == "corner_radius") {
        _style[key] = toInt(value, std::max(_parent_height, _parent_width));
      }
      else if(key == "padding") {
        std::tie(_style["padding_top"], _style["padding_right"], _style["padding_bottom"], _style["padding_left"]) = parsePaddingOrMargin(value);
      }
      else if(key == "margin") {
        std::tie(_style["margin_top"], _style["margin_right"], _style["margin_bottom"], _style["margin_left"]) = parsePaddingOrMargin(value);
      }
      else if(key == "border") {
        // auto [border_type, border_width, border_color] = LeleStyle::parseBorder(value);
        // _style["border_type"] = border_type;
        // _style["border_width"] = border_width;
        // _style["border_color"] = border_color;
        std::tie(_style["border_type"], _style["border_width"], _style["border_color"]) = LeleStyle::parseBorder(value); 
      }
      else if(key == "flow") {
        if(strncmp(value.c_str(), "row", 6)==0) {
          _style[key] = LV_FLEX_FLOW_ROW;
        }
        else if(strncmp(value.c_str(), "row_wrap", 8)==0) {
          _style[key] = LV_FLEX_FLOW_ROW_WRAP;
        }
        else if(strncmp(value.c_str(), "row_reverse", 11)==0){
          _style[key] = LV_FLEX_FLOW_ROW_REVERSE;
        }
        else if(strncmp(value.c_str(), "row_wrap_reverse", 16)==0){
          _style[key] = LV_FLEX_FLOW_ROW_WRAP_REVERSE;
        }
        else if(strncmp(value.c_str(), "column", 6)==0) {
          _style[key] = LV_FLEX_FLOW_COLUMN;//lv_obj_set_flex_flow(cont1, LV_FLEX_FLOW_COLUMN);
        }
        else if(strncmp(value.c_str(), "column_wrap", 11)==0){
          _style[key] = LV_FLEX_FLOW_COLUMN_WRAP;
        }
        else if(strncmp(value.c_str(), "column_reverse", 14)==0){
          _style[key] = LV_FLEX_FLOW_COLUMN_REVERSE;
        }
        else if(strncmp(value.c_str(), "column_wrap_reverse", 19)==0){
          _style[key] = LV_FLEX_FLOW_COLUMN_WRAP_REVERSE;
        }
        else {
          _style[key] = std::nullopt;
        }
      }
      else if(key == "fgcolor") {
        _style[key] = LeleStyle::parseColorCode(value);
      }
      else if(key == "bgcolor") {
        _style[key] = LeleStyle::parseColorCode(value);
      }
      else if(key == "checked_color") {
        _style[key] = LeleStyle::parseColorCode(value);
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
  if(padding_str.empty() || padding_str == "none" || padding_str == "0px") {
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

std::optional<LeleStyle::StyleValue> LeleStyle::getValue(const std::string &key, std::string class_name) const {
  std::string cls = class_name.empty() ? _class_name : class_name;
  if(cls == _class_name) {
    auto it = _style.find(key);
    if(it != _style.end() && it->second) {
      return it->second;
    }
  }
  if(_lele_parent) {
    return _lele_parent->styles()->getValue(key, cls);
  }
  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////
LeleStyles::LeleStyles(const std::string &json_str) {
  for (const auto &[key, token]: LeleWidgetFactory::fromJson(json_str)) {
    if (std::holds_alternative<std::unique_ptr<LeleStyle>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleStyle>>(token);
      if(key == "style") {
        _lele_styles.push_back(dynamic_cast<LeleStyle*> (value.get()));
      }
    }
    else if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "id") {
        _id = value;
      }
    }
  }
}
// lv_obj_t *LeleStyles::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {
//   setParent(lele_parent);
//   return _lv_obj;
// }
void LeleStyles::setLeleParent(LeleBase *lele_parent) {
    for(auto *lele_style : _lele_styles) {
      if(!lele_style->getLeleParent()) {
        lele_style->setLeleParent(lele_parent);
      }
    }
    _lele_parent = lele_parent;
}
// void LeleStyles::addStyle(LeleStyle* lele_style) {
//   _lele_styles.push_back(lele_style);
// }
LeleStyles &LeleStyles::operator+=(LeleStyles &lele_styles) {
  _lele_styles.insert(_lele_styles.end(), lele_styles._lele_styles.begin(), lele_styles._lele_styles.end());
  return *this;
}
LeleStyles &LeleStyles::operator+=(LeleStyle &lele_style) {
  _lele_styles.push_back(&lele_style);
  return *this;
}
std::optional<LeleStyle::StyleValue> LeleStyles::getValue(const std::string &key, std::string class_name) const {
  std::optional<LeleStyle::StyleValue> final_value;
  for(auto *lele_style : _lele_styles) {
    auto value = lele_style->getValue(key, class_name);
    if(value) {
      final_value = value;
    }
  }
  if(!final_value && _lele_parent) {
    final_value = _lele_parent->styles()->getValue(key, class_name);
  }
  return final_value;
}
////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const LeleStyle& p) {
    os << "LeleStyle id: " << p._id << ", {";
    os << "parent:" << (p._lele_parent ? p._lele_parent->id() : "") << ",";
    os << "parent class name:" << (p._lele_parent ? p._lele_parent->className() : "") << ",";
    for(const auto [style, value]: p._style) {
      if(value.has_value()) {
        os << style << ":";
        if (std::holds_alternative<int>(value.value())) {
          os << std::get<int>(value.value()) << ",";
        }
        else if (std::holds_alternative<std::string>(value.value())) {
          os << std::get<std::string>(value.value()) << ",";
        }
        else if (std::holds_alternative<lv_flex_flow_t>(value.value())) {
          os << std::get<lv_flex_flow_t>(value.value()) << ",";
        }
        else if (std::holds_alternative<LeleStyle::BorderTypeE>(value.value())) {
          os << std::get<LeleStyle::BorderTypeE>(value.value()) << ",";
        }
      }
    }
    os << "}\n";
    return os;
}
std::ostream& operator<<(std::ostream& os, const LeleStyles& p) {
    // os << "LeleStyles id: " << p._id << ", ";
    os << "parent:" << (p._lele_parent ? p._lele_parent->id() : "") << ",";
    os << "parent class name:" << (p._lele_parent ? p._lele_parent->className() : "") << ",";
    os << "\nStyles {\n";
    for(const LeleStyle *lele_style : p._lele_styles) {
      os << "\t" << *lele_style << ",\n";
    }
    os << "}\n";
    return os;
}