#include "lelebase.h"

LeleBase::LeleBase(const std::string &json_str, lv_obj_t *parent)
: _lv_parent_obj(parent) {
    _tokens = LeleWidgetFactory::fromJson(json_str);
}

LeleBase::~LeleBase() {
    lv_style_reset(&_style);
}

lv_obj_t *LeleBase::createLvObj(lv_obj_t *parent, int x, int y, int width, int height, const std::string &corner_radius) const {
  lv_style_init(&_style);
  if(!corner_radius.empty()) {
    lv_style_set_radius(&_style, std::stoi(corner_radius.c_str(), nullptr, 10));
  }
  lv_style_set_width(&_style, width);
  lv_style_set_height(&_style, height);
  lv_style_set_x(&_style, lv_pct(x));
  lv_style_set_y(&_style, y);
  lv_style_set_pad_ver(&_style, 20);
  lv_style_set_pad_left(&_style, 5);

  lv_obj_t *obj = lv_obj_create(parent);
  lv_obj_set_pos(obj, x, y);
  lv_obj_add_style(obj, &_style, 0);

  return obj;
}