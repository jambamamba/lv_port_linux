
#include "lelepos.h"
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

LelePos::LelePos(const std::string &json_str, lv_obj_t *parent) 
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
      else if(key == "fgcolor") {
        _fgcolor = std::stoi(value.c_str(), nullptr, 16);
      }
      else if(key == "bgcolor") {
        _bgcolor = std::stoi(value.c_str(), nullptr, 16);
      }
    }
  }
}
int LelePos::x() const {
  return toInt(_x, _parent_width);
}
int LelePos::y() const {
  return toInt(_y, _parent_height);
}
int LelePos::width() const {
  return toInt(_width, _parent_width);
}
int LelePos::height() const {
  return toInt(_height, _parent_height);
}
int LelePos::cornerRadius() const {
  return toInt(_corner_radius, std::max(_parent_height, _parent_width));  
}
int LelePos::padLeft() const {
  return toInt(_pad_left, _parent_width);  
}
int LelePos::padVer() const {
  return toInt(_pad_ver, _parent_height);
}
int LelePos::bgColor() const {
  return toInt(_pad_ver, _parent_height);
}