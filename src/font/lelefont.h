#pragma once

#include <lvgl/lvgl.h>
#include <unordered_map>
#include <string>

class LeleFont {
public:
  struct Font {
    std::string _lvf_file;
    lv_font_t * _lv_font = nullptr;
    Font(const std::string &lvf_file);
    ~Font();
  };
  const lv_font_t *getFont(const std::string &family="montserrat", int size=16);
protected:
  std::unordered_map<std::string, std::unordered_map<int, Font>> _font_db;
};
