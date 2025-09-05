#include "lelebase.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleBase::LeleBase(const std::string &json_str)
: _id(__func__ ) {

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

lv_obj_t *LeleBase::createLvObj(LeleBase *lele_parent) {

  setParent(lele_parent);
  
  lv_style_init(&_style);
  lv_style_set_radius(&_style, _pos->cornerRadius());
  lv_style_set_width(&_style, _pos->width());
  lv_style_set_height(&_style, _pos->height());
  lv_style_set_x(&_style, lv_pct(_pos->x()));
  lv_style_set_y(&_style, lv_pct(_pos->y()));
  lv_style_set_pad_ver(&_style, _pos->padVer());
  lv_style_set_pad_left(&_style, _pos->padLeft());

  if(_lv_obj) {
    lv_obj_del(_lv_obj);
  }
  _lv_obj = lv_obj_create(lele_parent->getLvObj());
  lv_obj_set_pos(_lv_obj, _pos->x(), _pos->y());
  lv_obj_add_style(_lv_obj, &_style, 0);
  LOG(DEBUG, LVSIM, "self:%s\n", getId().c_str());
  int fgcolor = _pos->fgColor();
  lv_obj_set_style_text_color(_lv_obj, lv_color_hex(fgcolor), LV_PART_MAIN);
  LOG(DEBUG, LVSIM, "---> got fgcolor: 0x%x\n", fgcolor);
  lv_obj_set_style_bg_color(_lv_obj, lv_color_hex(_pos->bgColor()), LV_PART_MAIN);

  return _lv_obj;
}
