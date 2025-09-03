#include "lelebase.h"

LOG_CATEGORY(LVSIM, "LVSIM");

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

  LOG(DEBUG, LVSIM, "@# (%i,%i)x(%i,%i), parent:(%i,%i)x(%i,%i)\n", 
    _pos->x(), _pos->y(), _pos->width(), _pos->height(),
    lv_obj_get_x(parent), lv_obj_get_y(parent), lv_obj_get_width(parent), lv_obj_get_height(parent)
  );
  // lv_style_reset(&_style);
  lv_style_init(&_style);
  lv_style_set_radius(&_style, _pos->cornerRadius());
  lv_style_set_width(&_style, _pos->width());
  lv_style_set_height(&_style, _pos->height());
  lv_style_set_x(&_style, lv_pct(_pos->x()));
  lv_style_set_y(&_style, _pos->y());
  lv_style_set_pad_ver(&_style, _pos->padVer());
  lv_style_set_pad_left(&_style, _pos->padLeft());

  if(_lv_obj) {
    lv_obj_del(_lv_obj);
  }
  _lv_obj = lv_obj_create(parent);
  lv_obj_set_pos(_lv_obj, _pos->x(), _pos->y());
  lv_obj_add_style(_lv_obj, &_style, 0);

  return _lv_obj;
}