#include "lelebase.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleBase::LeleBase(const std::string &json_str)
: _id(__func__ ) {

  _tokens = LeleWidgetFactory::fromJson(json_str);
  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::unique_ptr<LeleStyle>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleStyle>>(token);
      if(key == "pos") {
        _lele_style = dynamic_cast<LeleStyle*> (value.get());
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
  lv_style_set_radius(&_style, _lele_style->cornerRadius());
  lv_style_set_width(&_style, _lele_style->width());
  lv_style_set_height(&_style, _lele_style->height());
  lv_style_set_x(&_style, lv_pct(_lele_style->x()));
  lv_style_set_y(&_style, lv_pct(_lele_style->y()));
  lv_style_set_pad_ver(&_style, _lele_style->padVer());
  lv_style_set_pad_left(&_style, _lele_style->padLeft());

  if(_lv_obj) {
    lv_obj_del(_lv_obj);
  }
  _lv_obj = lv_obj_create(lele_parent->getLvObj());
  lv_obj_set_pos(_lv_obj, _lele_style->x(), _lele_style->y());
  lv_obj_add_style(_lv_obj, &_style, 0);
  // LOG(DEBUG, LVSIM, "self:%s\n", getId().c_str());
  int fgcolor = _lele_style->fgColor();
  lv_obj_set_style_text_color(_lv_obj, lv_color_hex(fgcolor), LV_PART_MAIN);
  // LOG(DEBUG, LVSIM, "---> got fgcolor: 0x%x\n", fgcolor);
  lv_obj_set_style_bg_color(_lv_obj, lv_color_hex(_lele_style->bgColor()), LV_PART_MAIN);

  return _lv_obj;
}
