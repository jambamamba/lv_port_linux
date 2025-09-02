
#include "lelepos.h"

LOG_CATEGORY(LVSIM, "LVSIM");

// LelePos LelePos::fromJson(int parent_width, int parent_height, const cJSON *json) {
//   std::string x, y, width, height;
//   cJSON *item = nullptr;
//   cJSON_ArrayForEach(item, json) {
//       // LOG(DEBUG, LVSIM, "tab content: %s\n", item->string);
//       if(strcmp(item->string, "x") == 0) {
//         x = cJSON_GetStringValue(item);
//       }
//       else if(strcmp(item->string, "y") == 0) {
//         y= cJSON_GetStringValue(item);
//       }
//       else if(strcmp(item->string, "width") == 0) {
//         width= cJSON_GetStringValue(item);
//       }
//       else if(strcmp(item->string, "height") == 0) {
//         height= cJSON_GetStringValue(item);
//       }
//     }
//     return LelePos(parent_width, parent_height, x, y, width, height);
// }

// LelePos::LelePos(int parent_width, int parent_height, const std::string &x, const std::string &y, const std::string &width, const std::string &height) 
// : _parent_width(parent_width)
// , _parent_height(parent_height) 
// , _x(x)
// , _y(y)
// , _width(width)
// , _height(height) {
// }

LelePos::LelePos(const std::string &json_str, lv_obj_t *parent) 
  : LeleBase(json_str, parent)
  , _parent_width(lv_obj_get_width(parent))
  , _parent_height(lv_obj_get_height(parent)) {

  for (const auto &[key, token]: _tokens) {
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
    }
  }
}

int LelePos::absFromPercent(int percent, int parent) const {
    return percent * parent / 100;
}
int LelePos::x() const {
    if(_x.size() > 0 && _x.c_str()[_x.size() - 1] == '%' && _parent_width > 0) {
        return absFromPercent(std::atoi(_x.c_str()), _parent_width);
    }
    else if(_x.size() > 0) {
        return std::atoi(_x.c_str());
    }
    return 0;
}
int LelePos::y() const {
    if(_y.size() > 0 && _y.c_str()[_y.size() - 1] == '%' && _parent_height > 0) {
        return absFromPercent(std::atoi(_y.c_str()), _parent_height);
    }
    else if(_y.size() > 0) {
        return std::atoi(_y.c_str());
    }
    return 0;
}
int LelePos::width() const {
    if(_width.size() > 0 && _width.c_str()[_width.size() - 1] == '%' && _parent_width > 0) {
        return absFromPercent(std::atoi(_width.c_str()), _parent_width);
    }
    else if(_width.size() > 0) {
        return std::atoi(_width.c_str());
    }
    return 0;
}
int LelePos::height() const {
    if(_height.size() > 0 && _height.c_str()[_height.size() - 1] == '%' && _parent_height > 0) {
        return absFromPercent(std::atoi(_height.c_str()), _parent_height);
    }
    else if(_height.size() > 0) {
        return std::atoi(_height.c_str());
    }
    return 0;
}
