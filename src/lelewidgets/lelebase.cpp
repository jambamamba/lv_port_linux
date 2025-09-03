#include "lelebase.h"

LeleBase::LeleBase(const std::string &json_str, lv_obj_t *parent) {

  _tokens = LeleWidgetFactory::fromJson(json_str);
  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::unique_ptr<LelePos>>(token)) {
      auto &value = std::get<std::unique_ptr<LelePos>>(token);
      if(key == "pos") {
        _pos = dynamic_cast<LelePos*> (value.get());
      }
    }
  }
}

LeleBase::~LeleBase() {
    lv_style_reset(&_style);
}

lv_obj_t *LeleBase::createLvObj(lv_obj_t *parent) {
  lv_style_init(&_style);
  lv_style_set_radius(&_style, _pos->cornerRadius());
  lv_style_set_width(&_style, _pos->width());
  lv_style_set_height(&_style, _pos->height());
  lv_style_set_x(&_style, lv_pct(_pos->x()));
  lv_style_set_y(&_style, _pos->y());
  lv_style_set_pad_ver(&_style, _pos->padVer());
  lv_style_set_pad_left(&_style, _pos->padLeft());

  lv_obj_t *obj = lv_obj_create(parent);
  lv_obj_set_pos(obj, _pos->x(), _pos->y());
  lv_obj_add_style(obj, &_style, 0);

  return obj;
}