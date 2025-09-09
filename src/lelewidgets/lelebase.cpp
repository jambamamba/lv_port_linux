#include "lelebase.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleBase::LeleBase(const std::string &json_str)
: _class_name(__func__ ) {

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
    // lv_style_reset(&_style);
}

namespace {
//Extending current theme:
// https://docs.lvgl.io/master/details/common-widget-features/styles/styles.html#extending-the-current-theme
#if 0
static void new_theme_apply_cb(lv_theme_t * th, lv_obj_t * obj)
{
    LV_UNUSED(th);

    if(lv_obj_check_type(obj, &lv_button_class)) {
        lv_obj_add_style(obj, &style_btn, 0);
    }
}

static void new_theme_init_and_set(void)
{
    /*Initialize the styles*/
    lv_style_init(&style_btn);
    lv_style_set_bg_color(&style_btn, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_border_color(&style_btn, lv_palette_darken(LV_PALETTE_GREEN, 3));
    lv_style_set_border_width(&style_btn, 3);

    /*Initialize the new theme from the current theme*/
    lv_theme_t * th_act = lv_display_get_theme(NULL);
    static lv_theme_t th_new;
    th_new = *th_act;

    /*Set the parent theme and the style apply callback for the new theme*/
    lv_theme_set_parent(&th_new, th_act);
    lv_theme_set_apply_cb(&th_new, new_theme_apply_cb);

    /*Assign the new theme to the current display*/
    lv_display_set_theme(NULL, &th_new);
}
#endif//0
}//namespace

void LeleBase::setStyle() {
  lv_style_init(&_style);
  lv_style_set_radius(&_style, _lele_style->cornerRadius());
  lv_style_set_width(&_style, _lele_style->width());
  lv_style_set_height(&_style, _lele_style->height());
  lv_style_set_x(&_style, lv_pct(_lele_style->x()));
  lv_style_set_y(&_style, lv_pct(_lele_style->y()));
  lv_style_set_pad_ver(&_style, _lele_style->padVer());
  lv_style_set_pad_left(&_style, _lele_style->padLeft());
  lv_style_set_border_color(&_style, lv_color_hex(_lele_style->borderColor()));
  lv_style_set_border_width(&_style, _lele_style->borderWidth());

  lv_obj_set_pos(_lv_obj, _lele_style->x(), _lele_style->y());
  lv_obj_add_style(_lv_obj, &_style, 0);
  lv_obj_set_style_text_color(_lv_obj, lv_color_hex(_lele_style->fgColor()), LV_PART_MAIN);
  lv_obj_set_style_bg_color(_lv_obj, lv_color_hex(_lele_style->bgColor()), LV_PART_MAIN);

  auto flow = _lele_style->flow();
  if(flow) {
    lv_obj_set_flex_flow(_lv_obj, flow.value());
  }
  // lv_theme_t * my_theme = lv_theme_create_from_default(lv_disp_get_default(), lv_color_hex(0x0000FF), lv_color_hex(0x00FF00)); // Create a new theme
  // lv_theme_set_active(my_theme); // Set the new theme as active

  // lv_style_t * btn_style = lv_theme_get_style(my_theme, LV_PART_MAIN); // Get the button style from your custom theme
  // lv_style_set_bg_color(btn_style, lv_color_hex(0x0000FF), LV_STATE_DEFAULT); // Customize the button style
}

lv_obj_t *LeleBase::createLvObj(LeleBase *lele_parent) {

  if(!_lv_obj) {
    _lv_obj = lv_obj_create(lele_parent->getLvObj());
  }
  setParent(lele_parent);
  setStyle();
  return _lv_obj;
}


void LeleBase::EventCallback(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    LeleBase *base = static_cast<LeleBase*>(e->user_data);
    if(base) {
      base->eventCallback(e);
    }
}

void LeleBase::eventCallback(lv_event_t * e) {
}