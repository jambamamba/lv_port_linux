#include "leletextbox.h"


LOG_CATEGORY(LVSIM, "LVSIM");

LeleTextbox::LeleTextbox(const std::string &json_str, lv_obj_t *parent)
  : LeleBase(json_str, parent) {

  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "text") {
        _text = value;
      }
      else if(key == "max_length") {
        _max_length = value.empty() ? 15 : std::atoi(value.c_str());
      }
      else if(key == "multiline") {
        _multiline = value.empty() ? true : (std::atoi(value.c_str()) != 0);
      }
    }
  }
}

lv_obj_t *LeleTextbox::createLvObj(lv_obj_t *parent) {

  _lv_obj = lv_textarea_create(
    LeleBase::createLvObj(parent));
  lv_textarea_set_text(_lv_obj, _text.c_str());
  // lv_obj_align(_lv_obj, LV_ALIGN_TOP_MID, x, y);
  lv_textarea_set_text_selection(_lv_obj, true);
  lv_textarea_set_max_length(_lv_obj, _max_length);
  lv_textarea_set_one_line(_lv_obj, _multiline);
  lv_obj_add_event_cb(_lv_obj, TextAreaEventCallback, LV_EVENT_CLICKED, _lv_obj);//also triggered when Enter key is pressed

  return _lv_obj;
}


// LeleTextbox::LeleTextbox(lv_obj_t *parent, const std::string &text, int x, int y, int width, int height, int corner_radius) 
//   : LeleBase() {
//     lv_style_init(&_style);
//     lv_style_set_radius(&_style, corner_radius);
//     lv_style_set_width(&_style, width);
//     lv_style_set_height(&_style, height);
//     lv_style_set_pad_ver(&_style, 20);
//     lv_style_set_pad_left(&_style, 5);
//     lv_style_set_x(&_style, lv_pct(x));
//     lv_style_set_y(&_style, y);

//     _lv_obj = lv_textarea_create(parent);
//     lv_obj_add_style(_lv_obj, &_style, 0);
//     lv_obj_set_pos(_lv_obj, x, y);

//     lv_textarea_set_text(_lv_obj, text.c_str());
//     // lv_obj_align(_lv_obj, LV_ALIGN_TOP_MID, x, y);
//     // lv_obj_set_size(_lv_obj, width, height);
//     lv_textarea_set_text_selection(_lv_obj, true);
//     lv_textarea_set_max_length(_lv_obj, 15);
//     lv_textarea_set_one_line(_lv_obj, true);
//     lv_obj_add_event_cb(_lv_obj, TextAreaEventCallback, LV_EVENT_CLICKED, _lv_obj);//also triggered when Enter key is pressed
// }

void LeleTextbox::TextAreaEventCallback(lv_event_t * e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * _ta = (lv_obj_t *)e->user_data;

    if(code == LV_EVENT_CLICKED 
      && lv_event_get_target(e)==_ta){
        const char * text = lv_textarea_get_text(_ta);
    }
}
