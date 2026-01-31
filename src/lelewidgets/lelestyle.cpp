
#include <algorithm>
#include <ranges>
#include <regex>
#include <image_builder/image_builder.h>

#include "lelestyle.h"
#include "lelewidgetfactory.h"
#include "leleobject.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleStyle::LeleStyle(LeleObject *lele_obj, const std::string &json_str) 
: _lele_obj(lele_obj) {

  fromJson(json_str);
}

//   LeleStyle::LeleStyle(LeleObject *lele_obj, const std::map<std::string, std::optional<LeleStyle::StyleValue>> &style_attributes, lv_obj_t *parent)
//   : _lele_obj(lele_obj) {

//   for(const auto &[key, value]: style_attributes) {
//     _style[key] = value;
//   }
// }
  
bool LeleStyle::fromJson(const std::string &json_str) {
  for (const auto &[key, token]: LeleWidgetFactory::fromJson(_lele_obj, json_str)) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(!setValue(key, value)) {
        return false;
      }
    }
  }
  return true;
}

int LeleStyle::parsePercentValue(const std::string &x, int parent_x) {
    if(x.size() > 0 && x.c_str()[x.size() - 1] == '%' && parent_x > 0) {
        int i = 0;
        if(x.size() > 2 && x.c_str()[0] == '0' && x.c_str()[1] == 'x') {
          i = std::stoi(x, 0, 16);
        }
        else {
          i = std::stoi(x, 0, 10);
        }
        // return absFromPercent(i, parent_x);
        return i * parent_x / 100;
    }
    else if(x.size() > 0) {
        int i = 0;
        if(x.size() > 2 && x.c_str()[0] == '0' && x.c_str()[1] == 'x') {
          i = std::stoi(x, 0, 16);
        }
        else if(std::all_of(x.begin(), x.end(),
          [&i](unsigned char ch){ return std::isdigit(ch); })) {
          i = std::stoi(x, 0, 10);
        }
        else {
          LL(WARNING, LVSIM) << "input is not a number: " << x;
        }
        return i;
    }
    return 0;
}

std::vector<std::string> LeleStyle::getBackgroundAttributesAsOrderedInJson() const {
  return _background_attributes_as_ordered_in_json;
}

std::map<std::string, std::vector<std::string>> LeleStyle::_flex_possible_values = {
  {"flow", {"row","column","row-wrap","column-wrap","row-reverse","column-reverse","row-wrap-reverse","column-wrap-reverse"}},
  {"justify-content", {"start","center","space-between","space-around","space-evenly"}},
  {"align-items", {"start","end","center"}},
  {"align-content", {"start","center","space-between","space-around"}}
};
std::map<std::string, std::map<std::string,int>> LeleStyle::_flex_possible_ivalues = {
  {"flow",{
      {"row",LV_FLEX_FLOW_ROW},
      {"column",LV_FLEX_FLOW_COLUMN},
      {"row-wrap",LV_FLEX_FLOW_ROW_WRAP},
      {"column-wrap",LV_FLEX_FLOW_COLUMN_WRAP},
      {"row-reverse",LV_FLEX_FLOW_ROW_REVERSE},
      {"column-reverse",LV_FLEX_FLOW_COLUMN_REVERSE},
      {"row-wrap-reverse",LV_FLEX_FLOW_ROW_WRAP_REVERSE},
      {"column-wrap-reverse",LV_FLEX_FLOW_COLUMN_WRAP_REVERSE}
    }
  },
  {"justify-content",{
      {"start",LV_FLEX_ALIGN_START},
      {"center",LV_FLEX_ALIGN_CENTER},
      {"space-between",LV_FLEX_ALIGN_SPACE_BETWEEN},
      {"space-around",LV_FLEX_ALIGN_SPACE_AROUND},
      {"space-evenly",LV_FLEX_ALIGN_SPACE_EVENLY}
    }
  },
  {"align-items", {
      {"start",LV_FLEX_ALIGN_START},
      {"end",LV_FLEX_ALIGN_END},
      {"center",LV_FLEX_ALIGN_CENTER}
    }
  },
  {"align-content", {
      {"start",LV_FLEX_ALIGN_START},
      {"center",LV_FLEX_ALIGN_CENTER},
      {"space-between",LV_FLEX_ALIGN_SPACE_BETWEEN},
      {"space-around",LV_FLEX_ALIGN_SPACE_AROUND}
    }
  }
};

void LeleStyle::parseFlex(const std::string &value_) {

  LeleWidgetFactory::fromJson(value_, [this](const std::string &subkey_, const std::string &value){
    const std::string key("flex");
    std::string subkey(subkey_);
    if(subkey == "flow") {
      if(_flex_possible_values[subkey].end() == std::find(_flex_possible_values[subkey].begin(), _flex_possible_values[subkey].end(), value)) {
        auto joined_view = _flex_possible_values[subkey] | std::views::join_with('|');//std::ranges::to<std::string>(joined_view);
        LL(WARNING, LVSIM) << "flex/" << subkey << "'" << value << "' is not valid. Acceptable values are: " << std::ranges::to<std::string>(joined_view);
        return;
      }
      _style[key + "/" + subkey] = value;
    }
    else if(subkey == "justify-content") {
      if(_flex_possible_values[subkey].end() == std::find(_flex_possible_values[subkey].begin(), _flex_possible_values[subkey].end(), value)) {
        auto joined_view = _flex_possible_values[subkey] | std::views::join_with('|');//std::ranges::to<std::string>(joined_view);
        LL(WARNING, LVSIM) << key << "/" << subkey << "'" << value << "' is not valid. Acceptable values are: " << std::ranges::to<std::string>(joined_view);
        return;
      }
      _style[key + "/" + subkey] = value;
    }
    else if(subkey == "align-items") {
      if(_flex_possible_values[subkey].end() == std::find(_flex_possible_values[subkey].begin(), _flex_possible_values[subkey].end(), value)) {
        auto joined_view = _flex_possible_values[subkey] | std::views::join_with('|');//std::ranges::to<std::string>(joined_view);
        LL(WARNING, LVSIM) << key << "/" << subkey << "'" << value << "' is not valid. Acceptable values are: " << std::ranges::to<std::string>(joined_view);
        return;
      }
      _style[key + "/" + subkey] = value;
    }
    else if(subkey == "align-content") {
      if(_flex_possible_values[subkey].end() == std::find(_flex_possible_values[subkey].begin(), _flex_possible_values[subkey].end(), value)) {
        auto joined_view = _flex_possible_values[subkey] | std::views::join_with('|');//std::ranges::to<std::string>(joined_view);
        LL(WARNING, LVSIM) << key << "/" << subkey << "'" << value << "' is not valid. Acceptable values are: " << std::ranges::to<std::string>(joined_view);
        return;
      }
      _style[key + "/" + subkey] = value;
    }
    else if(subkey == "grow") {
      if(std::all_of(value.begin(), value.end(),
        [this,&key,&subkey](unsigned char ch){ return std::isdigit(ch); })) {
        _style[key + "/" + subkey] = std::stoi(value, 0, 10);
      }
      else {
        LL(WARNING, LVSIM) << key << "/" << subkey << " is not a digit";        
      }
    }
    else {
      LL(WARNING, LVSIM) << key << "/" << subkey << " is not a valid attribute";        
      return;
    }
  });
}

std::tuple<LeleStyle::BorderTypeE,int,int> LeleStyle::parseBorder(const std::string &border_type_width_color) {

  LeleStyle::BorderTypeE border_type = LeleStyle::BorderTypeE::None;
  int border_width = 0;
  int border_color = 0;
  if(border_type_width_color.empty() ||
    border_type_width_color == "none") {
    return std::tuple<LeleStyle::BorderTypeE,int,int>{border_type, border_width, border_color};
  }
  std::regex pattern("(solid|dashed|dotted)-(\\d*)(px)?-(#[a-f0-9]{1,6}|\\d{1,10})$");
  std::smatch matches;
  //examples:
        // "solid-1px-#fff"
        // "solid-1px-#abcdef" // max 6 hex digit
        // "dashed-1px-0123456789"//[0-4294967296] max 10 digit 10 base number
        // "dotted-2-9999"
  if (std::regex_search(border_type_width_color, matches, pattern)
    && matches.size() == 5
  ) {
    if(matches[1] == "solid") {
      border_type = Solid;
    }
    else if(matches[1] == "dashed") {
      border_type = Dashed;
    }
    else if(matches[1] == "dotted") {
      border_type = Dotted;
    }
    border_width = std::stoi(matches[2].str());
    border_color = ImageBuilder::parseColorCode(matches[4]);
  }
  return std::tuple<LeleStyle::BorderTypeE,int,int>{border_type, border_width, border_color};
}

std::tuple<std::string,std::string,std::string,std::string> LeleStyle::parseTopRightBottomLeft(const std::string &json) {
  std::string top("none");
  std::string right("none");
  std::string bottom("none");
  std::string left("none");
  for (const auto &[key, val]: LeleWidgetFactory::fromJson(_lele_obj, json)) {
    if (std::holds_alternative<std::string>(val)) {
      const std::string &value = std::get<std::string>(val);
      if(value.empty() || value == "none" || value == "tight" || value == "parent" || value == "max") {
        if(key == "top") {
          // _style[key+"/top"] = val;
          // top = std::stoi(value);
          top = value;
        }
        else if(key == "right") {
          // _style[key+"/right"] = val;
          // right = std::stoi(value);
          right = value;
        }
        else if(key == "bottom") {
          // _style[key+"/bottom"] = val;
          // bottom = std::.stoi()(value);
          bottom = value;
        }
        else if(key == "left") {
          // _style[key+"/left"] = val;
          // left = std::.stoi()(value);
          left = value;
        }
      }
    }
  }
  return std::tuple<std::string,std::string,std::string,std::string>{top,right,bottom,left};
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
    top = std::stoi(matches[1].str());
    right = std::stoi(matches[2].str());
    bottom = std::stoi(matches[3].str());
    left = std::stoi(matches[4].str());
    return std::tuple<int,int,int,int>{top,right,bottom,left};
  }
  // std::string text("0px 0px");
  pattern = std::regex("(\\d*)px\\s+(\\d*)px\\s*");
  if (std::regex_search(padding_str, matches, pattern) &&
    matches.size() == 3) {
    top = bottom = std::stoi(matches[1].str());
    right = left = std::stoi(matches[2].str());
    return std::tuple<int,int,int,int>{top,right,bottom,left};
  }
  pattern = std::regex("(\\d*)px\\s*");
  if (std::regex_search(padding_str, matches, pattern) &&
    matches.size() == 2) {
    top = right = bottom = left = std::stoi(matches[1].str());
    return std::tuple<int,int,int,int>{top,right,bottom,left};
  }
  // std::string text("{"top":"0","right":"0","bottom":"0","left":"0"}");
  auto [top_str,right_str,bottom_str,left_str] = parseTopRightBottomLeft(padding_str.c_str());
  if(!top_str.empty() && top_str != "none"){ top = std::stoi(top_str); }
  if(!right_str.empty() && right_str != "none"){ right = std::stoi(right_str); }
  if(!bottom_str.empty() && bottom_str != "none"){ bottom = std::stoi(bottom_str); }
  if(!left_str.empty() && left_str != "none"){ left = std::stoi(left_str); }

  return std::tuple<int,int,int,int>{top,right,bottom,left};
}

std::string LeleStyle::getClassName() const {
  return _class_name;
}

const std::map<std::string, std::optional<LeleStyle::StyleValue>> LeleStyle::getStyle() const {
  return _style;
}

std::optional<LeleStyle::StyleValue> LeleStyle::getValue(const std::string &key, const std::string &class_name) const {
  if(class_name == _class_name || _class_name.empty()) {
    auto it = _style.find(key);
    if(it != _style.end() && it->second) {
      return it->second;
    }
  }
  if(_lele_obj && _lele_obj->getParent()) {
    return _lele_obj->getParent()->getStyle(key, class_name);
  }
  return std::nullopt;
}

bool LeleStyle::setValue(
    const std::string &key, 
    const std::string &value) {


    if(key == "class_name") {
        _class_name = value;
    }
    else if(key == "id") {
        _id = value;
    }
    // else if(
    //   _style.find(key) == _style.end() && 
    //   key != "padding" && 
    //   key != "margin" && 
    //   key != "border" && 
    //   key != "background"
    //   ){

    //   LOG(WARNING, LVSIM, "No such key ('%s') exists for styles", key.empty() ? "" : key.c_str());
    //   return false;
    // }
    else if(key == "x") {
      _style[key] = parsePercentValue(value, ImageBuilder::getParentDimension(key, _lele_obj));
    }
    else if(key == "y") {
      _style[key] = parsePercentValue(value, ImageBuilder::getParentDimension(key, _lele_obj));
    }
    else if(key == "width") {
      _style[key] = parsePercentValue(value, ImageBuilder::getParentDimension(key, _lele_obj));
    }
    else if(key == "height") {
      _style[key] = parsePercentValue(value, ImageBuilder::getParentDimension(key, _lele_obj));
    }
    else if(key == "corner_radius") {
      _style[key] = parsePercentValue(value, std::max(ImageBuilder::getParentDimension(key, _lele_obj), ImageBuilder::getParentDimension(key, _lele_obj)));
    }
    else if(key == "padding") {
      std::tie(_style["padding/top"], _style["padding/right"], _style["padding/bottom"], _style["padding/left"]) = parsePaddingOrMargin(value);
    }
    else if(key == "padding/top") {
      int top, right, bottom, left;
      std::tie(_style["padding/top"], right, bottom, left) = parsePaddingOrMargin(value);
    }
    else if(key == "padding/right") {
      int top, right, bottom, left;
      std::tie(top, _style["padding/right"], bottom, left) = parsePaddingOrMargin(value);
    }
    else if(key == "padding/bottom") {
      int top, right, bottom, left;
      std::tie(top, right, _style["padding/bottom"], left) = parsePaddingOrMargin(value);
    }
    else if(key == "padding/left") {
      int top, right, bottom, left;
      std::tie(top, right, bottom, _style["padding/left"]) = parsePaddingOrMargin(value);
    }
    else if(key == "margin") {
      std::tie(_style["margin/top"], _style["margin/right"], _style["margin/bottom"], _style["margin/left"]) = parsePaddingOrMargin(value);
    }
    else if(key == "margin/top") {
      int top, right, bottom, left;
      std::tie(_style["margin/top"], right, bottom, left) = parsePaddingOrMargin(value);
    }
    else if(key == "margin/right") {
      int top, right, bottom, left;
      std::tie(top, _style["margin/right"], bottom, left) = parsePaddingOrMargin(value);
    }
    else if(key == "margin/bottom") {
      int top, right, bottom, left;
      std::tie(top, right, _style["margin/bottom"], left) = parsePaddingOrMargin(value);
    }
    else if(key == "margin/left") {
      int top, right, bottom, left;
      std::tie(top, right, bottom, _style["margin/left"]) = parsePaddingOrMargin(value);
    }
    else if(key == "border") {
      std::tie(_style["border/type"], _style["border/width"], _style["border/color"]) = LeleStyle::parseBorder(value); 
    }
    else if(key == "border/type") {
      std::string type; int width,  color;
      std::tie(_style["border/type"], width, color) = LeleStyle::parseBorder(value); 
    }
    else if(key == "border/width") {
      std::string type; int width,  color;
      std::tie(type, _style["border/width"], color) = LeleStyle::parseBorder(value); 
    }
    else if(key == "border/color") {
      std::string type; int width,  color;
      std::tie(type, width, _style["border/color"]) = LeleStyle::parseBorder(value); 
    }
    else if(key == "scrollbar") {
      if(strncmp(value.c_str(), "off", 3)==0 || strncmp(value.c_str(), "none", 4)==0 || strncmp(value.c_str(), "false", 4)==0) {
        _style[key] = LV_SCROLLBAR_MODE_OFF;
      }
      else if(strncmp(value.c_str(), "on", 2)==0 || strncmp(value.c_str(), "always", 6)==0 || strncmp(value.c_str(), "true", 4)==0) {
        _style[key] = LV_SCROLLBAR_MODE_ON;
      }
      else if(strncmp(value.c_str(), "active", 6)==0) {
        _style[key] = LV_SCROLLBAR_MODE_ACTIVE;
      }
      else {// "auto"
        _style[key] = LV_SCROLLBAR_MODE_AUTO;
      }
    }
    else if(key == "fgcolor") {
      _style[key] = ImageBuilder::parseColorCode(value);
    }
    else if(key == "bgcolor") {
      _style[key] = ImageBuilder::parseColorCode(value);
    }
    else if(key == "checked_color") {
      _style[key] = ImageBuilder::parseColorCode(value);
    }
    else if(key == "align") {
      if(strncmp(value.c_str(), "center", strlen("center"))==0){
        _style[key] = LV_ALIGN_CENTER;
      }
      else if(strncmp(value.c_str(), "top_left", strlen("top_left"))==0){
        _style[key] = LV_ALIGN_TOP_LEFT;
      }
      else if(strncmp(value.c_str(), "top_mid", strlen("top_mid"))==0){
        _style[key] = LV_ALIGN_TOP_MID;
      }
      else if(strncmp(value.c_str(), "top_right", strlen("top_right"))==0){
        _style[key] = LV_ALIGN_TOP_RIGHT;
      }
      else if(strncmp(value.c_str(), "bottom_left", strlen("bottom_left"))==0){
        _style[key] = LV_ALIGN_BOTTOM_LEFT;
      }
      else if(strncmp(value.c_str(), "bottom_mid", strlen("bottom_mid"))==0){
        _style[key] = LV_ALIGN_BOTTOM_MID;
      }
      else if(strncmp(value.c_str(), "bottom_right", strlen("bottom_right"))==0){
        _style[key] = LV_ALIGN_BOTTOM_RIGHT;
      }
      else if(strncmp(value.c_str(), "left_mid", strlen("left_mid"))==0){
        _style[key] = LV_ALIGN_LEFT_MID;
      }
      else if(strncmp(value.c_str(), "right_mid", strlen("right_mid"))==0){
        _style[key] = LV_ALIGN_RIGHT_MID;
      }
      else {
        _style[key] = LV_ALIGN_DEFAULT;
      }
    }
    else if(key == "text-align") {
      if(strncmp(value.c_str(), "center", 6)==0){
        _style[key] = LV_TEXT_ALIGN_CENTER;
      }
      else if(strncmp(value.c_str(), "right", 5)==0){
        _style[key] = LV_TEXT_ALIGN_RIGHT;
      }
      else if(strncmp(value.c_str(), "left", 4)==0){
        _style[key] = LV_TEXT_ALIGN_LEFT;
      }
      else {
        _style[key] = LV_TEXT_ALIGN_AUTO;
      }
    }
    else if(key == "flex") {
      parseFlex(value);
    }
    else if(key == "background") {
      auto [style, attributes] = ImageBuilder::parseBackground(key, value, _lele_obj);
      _style.merge(style);
      _background_attributes_as_ordered_in_json = attributes;
    }
    else if(key == "background/rotation/angle") {
      _style["background/rotation/angle"] = std::stof(value);
    }
    else if(key == "background/rotation/pivot") {
      int pivot_x = 0;
      int pivot_y = 0;
      int max_x = ImageBuilder::getParentDimension("width", _lele_obj);
      int max_y = ImageBuilder::getParentDimension("height", _lele_obj);
      if(LeleWidgetFactory::parsePercentValues(
        value, 
        {{"x", &pivot_x}, {"y", &pivot_y}},
        {{"x", max_x}, {"y", max_y}}
      )) {//osm todo: need max_x, max_y to parse x y if they are percentages
        _style["background/rotation/pivot/x"] = pivot_x;
        _style["background/rotation/pivot/y"] = pivot_y;
      }
    }
    else if(key == "background/rotation/pivot/x") {
      _style["background/rotation/pivot/x"] = std::stoi(value);
    }
    else if(key == "background/rotation/pivot/y") {
      _style["background/rotation/pivot/y"] = std::stoi(value);
    }
    else if(key == "background/color") {
        _style["background/color"] = ImageBuilder::parseColorCode(value);
    }
    else if(key == "background/image") {
        _style["background/image"] = value;
    }
    else if(key == "background/position") {
        _style["background/position"] = value;
    }
    else if(key == "background/size") {
        _style["background/size"] = value;
    }
    else if(key == "background/repeat") {
        _style["background/repeat"] = value;
    }
    else if(key.at(0) == '#' || key.at(0) == '@') {
      LL(WARNING, LVSIM) << "Ignoreing key " << key << " for styles";
      return false;
    }
    else {
      LL(FATAL, LVSIM) << "No such key " << key << " exists for styles";
      return false;
    }
    return true;
}

std::string LeleStyle::getId() const {
  return _id;
}

void LeleStyle::applyStyle() {
  if(_lele_obj) {
    (const_cast<LeleObject *>(_lele_obj))->
      setStyle(_lele_obj->getLvObj());
  }
}

const LeleObject *LeleStyle::getLeleObject() const { 
  return _lele_obj; 
}

std::ostream& operator<<(std::ostream& os, const LeleStyle& p) {
    os << "LeleStyle id: " << p._id << ", {";
    // os << "parent:" << (p._lele_obj ? p._lele_obj->id() : "") << ",";
    // os << "parent class name:" << (p._lele_obj ? p._lele_obj->className() : "") << ",";
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
// std::ostream& operator<<(std::ostream& os, const LeleStyles& p) {
//     // os << "LeleStyles id: " << p._id << ", ";
//     // os << "parent:" << (p._lele_obj ? p._lele_obj->id() : "") << ",";
//     // os << "parent class name:" << (p._lele_obj ? p._lele_obj->className() : "") << ",";
//     os << "\nStyles {\n";
//     for(const LeleStyle *lele_style : p._lele_styles) {
//       os << "\t" << *lele_style << ",\n";
//     }
//     os << "}\n";
//     return os;
// }