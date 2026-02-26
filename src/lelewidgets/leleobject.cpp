#include <algorithm>
#include <font/lelefont.h>
#include <iostream>
#include <ranges>
#include <algorithm>
#include <image_builder/image_builder.h>
#include <lvgl/lvgl_private.h>
#include <lvgl/src/core/lv_obj_tree.h>
#include <python/python_wrapper.h>

#include "leleobject.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleObject::LeleObject(LeleObject *parent, const std::string &json_str)
  : _lele_parent(parent), 
  _class_name(__func__ ) {
  fromJson(json_str);
  // std::cout << "styles:" << _lele_styles << "\n";
}

LeleObject::~LeleObject() {
    // lv_style_reset(&_style);
}

void LeleObject::parseAttributes(
    const std::vector<std::pair<std::string, std::string>> &json_tokens) {

    for(const auto &[lhs, rhs]: json_tokens) {
        if(lhs == "id") {
          _id = rhs;
        }
        else if(lhs == "enabled") {
          _enabled = (rhs == "true");
        }
        else if(lhs == "style") {
          _lele_styles.emplace_back(
              std::make_unique<LeleStyle>(this, rhs));
        }
    }
}

bool LeleObject::fromJson(const std::string &json_str) {

  auto json_tokens = LeleWidgetFactory::tokenizeJson(json_str);
  parseAttributes(json_tokens);
  _nodes = LeleWidgetFactory::fromJson(this, json_tokens);
  return true;
}

const std::string &LeleObject::className() const {
  return _class_name;
}
const std::string &LeleObject::getId() const {
  return _id;
}
void LeleObject::setId(const std::string &id) {
  _id = id;
}

bool LeleObject::enabled() const { 
  return _enabled; 
}

lv_obj_t *LeleObject::getLvObj() const {
  return _lv_obj;
}

void LeleObject::setLvObj(lv_obj_t *obj) {
  obj->user_data = this;
  _lv_obj = obj;
}

void LeleObject::setParent(LeleObject *parent) {
  _lele_parent = parent;
}

LeleObject *LeleObject::getParent() const{
  return _lele_parent;
}

// const LeleStyles *LeleObject::styles() const {
//   return &_lele_styles;
// }

std::vector<std::pair<std::string, LeleWidgetFactory::Node>> &LeleObject::children() {
  return _nodes;
}

namespace {
//Extending current theme:
// https://docs.lvgl.io/master/details/common-widget-features/styles/styles.html#extending-the-current-theme
#if 0
static void new_theme_apply_cb(lv_theme_t * th, lv_obj_t * obj)
{
    LV_UNUSED(th);

    if(lv_obj_check_type(obj, &lv_button_class)) {
        lv_obj_add_style(obj, &style_btn, LV_PART_MAIN);
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

std::map<std::string, std::optional<LeleStyle::StyleValue>> LeleObject::getStyleAttributes(const std::string &style_id) const {

  std::map<std::string, std::optional<LeleStyle::StyleValue>> ret;
  for(const auto &lele_style : std::ranges::views::reverse(_lele_styles)) {
    if(!style_id.empty() && style_id != lele_style->getId()) {
      continue;
    }
    std::vector<std::string> keys;
    for(const auto &[key, value] : lele_style->getStyle()) {
      keys.push_back(key);
    }
    for(const auto &key : keys) {
      auto value = lele_style->getValue(key, lele_style->getClassName());
      if(value && ret.find(key) == ret.end()) {
        ret[key] = value;
      }
    }
  }
  return ret;
}

const std::vector<std::unique_ptr<LeleStyle>> &LeleObject::getStyles() const {
  return _lele_styles;
}
std::optional<LeleStyle::StyleValue> LeleObject::getStyle(const std::string &key, const std::string &class_name_) const {

  auto value = std::optional<LeleStyle::StyleValue>();
  std::vector<std::string> class_names;
  for(const auto &lele_style : std::ranges::views::reverse(_lele_styles)) {
    std::string class_name(class_name_);
    if(class_name.empty()) {
      class_name = lele_style->getClassName();
      class_names.push_back(class_name);
    }
    value = lele_style->getValue(key, class_name);
    if(value) {
      if(_id == "tab_content2" && key == "bgcolor") {
        int x = 0;
        x = 1;
        LL(DEBUG, LVSIM) << "@@@ getStyle: key:" << key << ", class_name:" << class_name << ", value:" << std::hex << std::get<int>(value.value());
      }
      return value;
    }
  }
  if(_lele_parent && class_names.empty()) {
    value = _lele_parent->getStyle(key, class_name_);
  }
  return value;
}

bool LeleObject::visitLvChildren(lv_obj_t *lv_obj, std::function<bool(lv_obj_t *)>callback) {
  for(int idx = 0; idx < lv_obj_get_child_count(lv_obj); ++idx) {
      lv_obj_t *child_obj = lv_obj_get_child(lv_obj, idx);
      if(!callback(child_obj)) {
        return false;
      }
      if(!visitLvChildren(child_obj, callback)) {
        return false;
      }
  }
  return true;
}

std::tuple<std::vector<std::string> ,std::map<std::string, std::optional<LeleStyle::StyleValue>>> 
LeleObject::getBackgroundStyle(const std::string &class_name) const {

  std::vector<std::string> bg_keys;
  std::map<std::string, std::optional<LeleStyle::StyleValue>> bg_style;
  for(const auto &lele_style : std::ranges::views::reverse(_lele_styles)) {
    for(const std::string &key : lele_style->getBackgroundAttributesAsOrderedInJson()) {
      std::string bg_key("background/" + key);
      auto value = lele_style->getValue(bg_key, class_name.empty() ? lele_style->getClassName() : class_name);
      if(value && bg_style.find(bg_key) == bg_style.end()) {
        bg_style[bg_key] = value;
        bg_keys.push_back(bg_key);
      }
    }
  }
  return {bg_keys, bg_style};
}

void LeleObject::applyStyle(lv_obj_t *lv_obj) {
  lv_style_init(&_style);

  int obj_x = 0;
  auto value = getStyle("x");
  if(value) {
    obj_x = std::get<int>(value.value());
    lv_obj_set_x(lv_obj, obj_x);
  }
  int obj_y = 0;
  value = getStyle("y");
  if(value) {
    obj_y = std::get<int>(value.value());
    lv_obj_set_y(lv_obj, obj_y);
  }
  int obj_width = -1;
  value = getStyle("width");
  if(value) {
    obj_width = std::get<int>(value.value());
    lv_obj_set_width(lv_obj, obj_width);
  }
  int obj_height = -1;
  value = getStyle("height");
  if(value) {
    obj_height = std::get<int>(value.value());
    lv_obj_set_height(lv_obj, obj_height);
  }

  value = getStyle("corner-radius");
  if(value) {
    lv_style_set_radius(&_style, std::get<int>(value.value()));
  }
  else {
    lv_style_set_radius(&_style, 0);
  }

  value = getStyle("padding/top");
  if(value) {
    lv_style_set_pad_top(&_style, std::get<int>(value.value()));
  }
  value = getStyle("padding/right");
  if(value) {
    lv_style_set_pad_right(&_style, std::get<int>(value.value()));
  }
  value = getStyle("padding/bottom");
  if(value) {
    lv_style_set_pad_bottom(&_style, std::get<int>(value.value()));
  }
  value = getStyle("padding/left");
  if(value) {
    lv_style_set_pad_left(&_style, std::get<int>(value.value()));
  }

  value = getStyle("margin/top");
  if(value) {
    lv_style_set_margin_top(&_style, std::get<int>(value.value()));
  }
  value = getStyle("margin/right");
  if(value) {
    lv_style_set_margin_right(&_style, std::get<int>(value.value()));
  }
  value = getStyle("margin/bottom");
  if(value) {
    lv_style_set_margin_bottom(&_style, std::get<int>(value.value()));
  }
  value = getStyle("margin/left");
  if(value) {
    lv_style_set_margin_left(&_style, std::get<int>(value.value()));
  }

  value = getStyle("border/type");
  if(!value || std::get<LeleStyle::BorderTypeE>(value.value()) == LeleStyle::BorderTypeE::None) {
    lv_style_set_border_width(&_style, 0);
  }
  else {
    value = getStyle("border/color");
    if(value) {
      lv_style_set_border_color(&_style, lv_color_hex(std::get<int>(value.value())));
    }
    value = getStyle("border/width");
    if(value) {
      lv_style_set_border_width(&_style, std::get<int>(value.value()));
    }
  }

  value = getStyle("fgcolor-selected");
  if(value) {
    lv_obj_set_style_text_color(lv_obj, lv_color_hex(std::get<int>(value.value())), LV_PART_SELECTED);
  }
  value = getStyle("bgcolor-selected");
  if(value) {
    lv_obj_set_style_bg_color(lv_obj, lv_color_hex(std::get<int>(value.value())), LV_PART_SELECTED);
  }
  value = getStyle("fgcolor");
  if(value) {
    lv_obj_set_style_text_color(lv_obj, lv_color_hex(std::get<int>(value.value())), LV_PART_MAIN);
  }
  value = getStyle("bgcolor");
  if(value) {
    int bgcolor = std::get<int>(value.value());
    lv_obj_set_style_bg_color(lv_obj, lv_color_hex(std::get<int>(value.value())), LV_PART_MAIN);
  }
  value = getStyle("align");
  if(value) {
    lv_align_t align = static_cast<lv_align_t>(std::get<int>(value.value()));
    lv_obj_align(lv_obj,
      static_cast<lv_align_t>(std::get<int>(value.value())), obj_x, obj_y);
  }
  value = getStyle("text-align");
  if(value) {
    lv_text_align_t align_type = static_cast<lv_text_align_t>(std::get<int>(value.value()));
    lv_obj_set_style_text_align(lv_obj, align_type, 0);
  }
  setFlexStyle();

  int font_size = 16;
  value = getStyle("font-size");
  if(value) {
    font_size = std::get<int>(value.value());
  }
  std::string font_family = "montserrat";
  value = getStyle("font-family");
  if(value) {
    font_family = std::get<std::string>(value.value());
  }
  // lv_obj_set_style_text_font(lv_obj, LeleStyle::getFont(font_family, font_size), LV_PART_MAIN);
  lv_style_set_text_font(&_style, LeleFont::getLeleFont().getFont(font_family, font_size));
  // lv_style_set_text_font(&_style, &lv_font_dejavu_16_persian_hebrew);

  value = getStyle("background/color");
  if(value) {
    _bg_color = ImageBuilder::fillBackgroundColor(std::get<int>(value.value()), obj_width, obj_height);
    if(!_lv_bg_color) {
      _lv_bg_color = lv_image_create(lv_obj);
    }
    if(!_lv_bg_color) {
        LOG(FATAL, LVSIM, "Failed in lv_image_create");
        return;
    }
    lv_image_set_src(_lv_bg_color, _bg_color.value().get());
  }
  value = getStyle("background/image");
  if(value) {
    auto [style_keys, style_map] = getBackgroundStyle();
    auto res = ImageBuilder::drawBackgroundImage(
      "background",
      std::get<std::string>(value.value()), 
      style_keys,
      style_map,
      obj_width,
      obj_height);
    if(!_lv_bg_img) {
      _lv_bg_img = lv_image_create(_lv_bg_color ? _lv_bg_color : _lv_obj);
    }
    if(!_lv_bg_img) {
        LOG(FATAL, LVSIM, "Failed in lv_image_create");
        return;
    }
    _bg_img = std::move(res._img_dsc);
    lv_image_set_src(_lv_bg_img, _bg_img.value().get());
  }
  value = getStyle("scrollbar");
  if(value) {
    lv_obj_set_scrollbar_mode(lv_obj, std::get<lv_scrollbar_mode_t>(value.value()));
  }
  
  // lv_theme_t * my_theme = lv_theme_create_from_default(lv_disp_get_default(), lv_color_hex(0x0000FF), lv_color_hex(0x00FF00)); // Create a new theme
  // lv_theme_set_active(my_theme); // Set the new theme as active

  // lv_style_t * btn_style = lv_theme_get_style(my_theme, LV_PART_MAIN); // Get the button style from your custom theme
  // lv_style_set_bg_color(btn_style, lv_color_hex(0x0000FF), LV_STATE_DEFAULT); // Customize the button style
  lv_obj_add_style(lv_obj, &_style, LV_PART_MAIN);
}

namespace {
std::optional<int> parseFlexValue(const std::optional<LeleStyle::StyleValue> &value, const std::string &key) {

  if(!value) {
    return std::nullopt;
  }
  std::string desired = std::get<std::string>(value.value());
  for(const std::string &allowed : LeleStyle::_flex_possible_values[key]) {
    if(desired == allowed) {
          int value_ = LeleStyle::_flex_possible_ivalues[key][desired];
          return value_;
    }
  }
  return std::nullopt;
}
}//namespace

void LeleObject::setFlexStyle() {
  int flow = LeleStyle::_flex_possible_ivalues["flow"]["row"];//default
  auto value = parseFlexValue(getStyle("flex/flow"), "flow");
  if(!value) {
    return;
  }
  flow = value.value();
  int justify_content = LeleStyle::_flex_possible_ivalues["justify-content"]["start"];//default
  value = parseFlexValue(getStyle("flex/justify-content"), "justify-content");
  if(value) {
    justify_content = value.value();
  }
  int align_items = LeleStyle::_flex_possible_ivalues["align-items"]["start"];//default
  value = parseFlexValue(getStyle("flex/align-items"), "align-items");
  if(value) {
    align_items = value.value();
  }
  int align_content = LeleStyle::_flex_possible_ivalues["align-content"]["start"];//default
  value = parseFlexValue(getStyle("flex/align-content"), "align-content");
  if(value) {
    align_content = value.value();
  }
  lv_obj_set_flex_flow(_lv_obj,
    static_cast<lv_flex_flow_t>(flow));
  lv_obj_set_flex_align(_lv_obj, 
    static_cast<lv_flex_align_t>(justify_content), 
    static_cast<lv_flex_align_t>(align_items), 
    static_cast<lv_flex_align_t>(align_content)
  );
}

void LeleObject::addStyle(std::vector<std::unique_ptr<LeleStyle>> &lele_styles) {
  if(lele_styles.size()){
    for(std::unique_ptr<LeleStyle> &lele_style : lele_styles) {
      _lele_styles.emplace_back(lele_style.get());
    }
    applyStyle(_lv_obj);
  }
}

void LeleObject::addStyle(LeleStyle* lele_style) {
  _lele_styles.emplace_back(lele_style);
  applyStyle(_lv_obj);
}

void LeleObject::removeStyle(const std::string &style_id) {
  //osm todo
  _lele_styles.clear();
}

std::pair<int,int> LeleObject::getTextSize(lv_obj_t *lv_obj, const char *text) {

    const lv_font_t *font = lv_obj_get_style_text_font(lv_obj, LV_PART_MAIN);
    lv_point_t text_size;
    int32_t letter_space = 0;
    int32_t line_space = 0;
    int32_t max_width = lv_obj_get_width(lv_screen_active());
    lv_text_get_size(&text_size, text, font, letter_space, line_space, max_width, LV_TEXT_FLAG_NONE);
    return std::pair<int,int>(text_size.x, text_size.y);
}


// void LeleObject::setObjAlignStyle(lv_obj_t *lv_obj) {
//   auto value = getStyle("align");
//   if(value) {
//     lv_obj_align(lv_obj, 
//       static_cast<lv_align_t>(
//         std::get<int>(value.value())),
//       0, 0);
//     //make sure to do lv_obj_set_layout(parent, LV_LAYOUT_NONE); otherwise this function might not work
//   }
// }

// void LeleObject::setTextAlign(lv_obj_t *lv_obj) {

//   auto value = getStyle("text-align");
//   if(value) {
//     lv_obj_set_style_text_align(lv_obj, 
//       static_cast<lv_text_align_t>(
//         std::get<int>(value.value())),
//       // LV_PART_ANY);
//       LV_PART_MAIN);
//   }
// }

// void setTextAlign(lv_obj_t *lv_obj, lv_text_align_t align_type) {

//   const lv_obj_class_t *child_class = lv_obj_get_class(lv_obj);
//   if(strcmp((char*)child_class->name, "label")==0) {
//     lv_obj_set_style_text_align(lv_obj, align_type, 0);
//   }
// }


lv_obj_t *LeleObject::createLvObj(LeleObject *lele_parent, lv_obj_t *lv_obj) {

  if(!lv_obj) {
    _lv_obj = lv_obj_create(lele_parent->getLvObj());
  }
  else {
    _lv_obj = lv_obj;
  }
  lv_obj_null_on_delete(&_lv_obj);
  setParent(lele_parent);
  applyStyle(_lv_obj);
  _lv_obj->user_data = this;
  lv_obj_add_event_cb(_lv_obj, EventCallback, LV_EVENT_ALL, this);
  return _lv_obj;
}

std::vector<LeleObject *> LeleObject::getLeleObj(const std::string &obj_name) const {
  std::vector<LeleObject *> res;
  for(const auto &pair: _nodes) {
    if (std::holds_alternative<std::unique_ptr<LeleObject>>(pair.second)) {
      auto &value = std::get<std::unique_ptr<LeleObject>>(pair.second);
      if(pair.first == obj_name) {
        res.push_back(value.get());
      }
    }
  }
  return res;
}

void LeleObject::EventCallback(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    LeleObject *base = static_cast<LeleObject*>(e->user_data);
    if(base) {
      // LL(DEBUG, LVSIM) << "LeleObject::eventCallback " <<
      //   "id:" << base->id() << 
      //   ", class_name: " << base->className() <<
      //   ", _lele_parent: " << base->getParent()->className() <<
      //   ", event_code: " << e->code <<
      //   " " << lv_event_code_get_name(e->code);
      base->eventCallback(LeleEvent(e, base));
    }
    // if(e->current_target) {
    //   lv_obj_t *obj = static_cast<lv_obj_t *>(e->current_target);
    //   if(obj && obj->user_data) {
    //     base = static_cast<LeleObject*>(obj->user_data);
    //   }
    // }
    // if(base) {
    //   base->eventCallback(LeleEvent(e, base));
    // }
}

void LeleObject::hide() {
  // int width = lv_obj_get_width(getLvObj());
  // int height = lv_obj_get_height(getLvObj());
  // lv_obj_set_size(getLvObj(), 0, 0);
  lv_obj_add_flag(getLvObj(), LV_OBJ_FLAG_HIDDEN);
}

void LeleObject::show() {
  lv_obj_remove_flag(getLvObj(), LV_OBJ_FLAG_HIDDEN);
}

bool LeleObject::eventCallback(LeleEvent &&e) {
  // LOG(DEBUG, LVSIM, "LeleObject::eventCallback id:%s, class_name:%s, _lele_parent:%s, code:[0x%x]%s,\n", 
  //   _id.c_str(), _class_name.c_str(), _lele_parent ? _lele_parent->className().c_str() : "", 
  //   e.getLvEvent()->code, lv_event_code_get_name(e.getLvEvent()->code));
  for(auto *py_callback:_py_callbacks) {
    if(!pyCallback(py_callback, std::move(e))) {
      return false;
    }
  }
  if(_lele_parent) {
    return _lele_parent->eventCallback(std::move(e));
  }
  return true;
}
void LeleObject::addEventHandler(PyObject *py_callback) {
  _py_callbacks.push_back(py_callback);
}
std::ostream& operator<<(std::ostream& os, const LeleObject& p) {
    // os << "LeleStyles id: " << p._id << ", ";
    os << "_id:" << p._id << ",";
    os << "_class_name:" << p._class_name << ",";
    os << "\nStyles {\n";
    for(const auto &lele_style : p._lele_styles) {
      os << lele_style.get();
    }
    return os;
}
