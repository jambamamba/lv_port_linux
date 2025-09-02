#include "lelelabel.h"

#include "lelepos.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleLabel::LeleLabel(const std::string &json_str, lv_obj_t *parent)
  : LeleBase(json_str, parent) {

  std::string text, corner_radius, bgcolor;
  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::unique_ptr<LeleBase>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleBase>>(token);
      if(key == "pos") {
        LelePos *pos = dynamic_cast<LelePos*> (value.get());
        pos;//osm
      }
    }
    else if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "text") {
        text = value;
      }
      else if(key == "corner_radius") {
        corner_radius = value;
      }
      else if(key == "bgcolor") {
        bgcolor = value;
      }
    }
  }

  //osm todo: get the hard coded values from config.json:
  lv_style_init(&_style);
  lv_style_set_radius(&_style, std::stoi(corner_radius.c_str(), nullptr, 10));
  // lv_style_set_width(&_style, width);
  // lv_style_set_height(&_style, height);
  // lv_style_set_x(&_style, lv_pct(x));
  // lv_style_set_y(&_style, y);
  // lv_style_set_pad_ver(&_style, 20);
  // lv_style_set_pad_left(&_style, 5);

  // lv_obj_t *obj = lv_obj_create(parent);
  // lv_obj_set_pos(obj, x, y);
  // lv_obj_add_style(obj, &_style, 0);

  // _lv_obj = lv_label_create(obj);
  // lv_label_set_text(_lv_obj, text.c_str());
}

// LeleLabel::LeleLabel(lv_obj_t *parent, const std::string &text, int x, int y, int width, int height, int corner_radius)
//   : LeleBase() {
//     lv_style_init(&_style); 
//     lv_style_set_radius(&_style, corner_radius);
//     lv_style_set_width(&_style, width);
//     lv_style_set_height(&_style, height);
//     lv_style_set_pad_ver(&_style, 20);
//     lv_style_set_pad_left(&_style, 5);
//     lv_style_set_x(&_style, lv_pct(x));
//     lv_style_set_y(&_style, y);

//     lv_obj_t *obj = lv_obj_create(parent);
//     lv_obj_set_pos(obj, x, y);
//     lv_obj_add_style(obj, &_style, 0);

//     _lv_obj = lv_label_create(obj);
//     lv_label_set_text(_lv_obj, text.c_str());
// }
