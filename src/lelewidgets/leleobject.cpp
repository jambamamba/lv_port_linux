#include <algorithm>
#include <iostream>
#include <ranges>
#include <algorithm>
#include <lvgl/lvgl_private.h>
#include </repos/lv_port_linux/lvgl/src/core/lv_obj_tree.h>

#include "leleobject.h"
#include "python_wrapper.h"

LOG_CATEGORY(LVSIM, "LVSIM");

namespace {
std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> resizeImageWithValuesParsedFromJson(
  const lv_image_dsc_t *src_img, std::string val, int container_width, int container_height) {

  int x = -1;
  int y = -1;
  val = LeleWidgetFactory::trim(val);
  if(LeleWidgetFactory::parsePercentValues(
    val, 
    {{"x", &x}, {"y", &y}}, 
    {{"x", container_width}, {"y", container_height}})) {
    return LeleImageConverter::resizeImg(src_img, x, y);
  }
  x = LeleStyle::parsePercentValue(val, container_width);
  y = LeleStyle::parsePercentValue(val, container_height);
  return LeleImageConverter::resizeImg(src_img, x, y);
}

std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> resizeContentToFillContainerPotentiallyCroppingContent(
  const lv_image_dsc_t *src_img, int container_width, int container_height) {
  int img_width = src_img->header.w;
  int img_height = src_img->header.h;
  int dx = std::abs(container_width - img_width);
  int dy = std::abs(container_height - img_height);
  int new_width = -1;
  int new_height = -1;
  if(dx < dy) {
    new_width = container_width;
    new_height = img_height * container_width / img_width;
  }
  else {
    new_height = container_height;
    new_width = img_width * container_height / img_height;
  }
  return LeleImageConverter::resizeImg(src_img, new_width, new_height);
}

std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> resizeToShowEntireContentPotentiallyLeavingEmptySpace(
  const lv_image_dsc_t *src_img, int container_width, int container_height) {
  int img_width = src_img->header.w;
  int img_height = src_img->header.h;
  int dx = std::abs(container_width - img_width);
  int dy = std::abs(container_height - img_height);
  int new_width = -1;
  int new_height = -1;
  if(dx < dy) {
    new_height = container_height;
    new_width = img_width * container_height / img_height;
  }
  else {
    new_width = container_width;
    new_height = img_height * container_width / img_width;
  }
  return LeleImageConverter::resizeImg(src_img, new_width, new_height);
}

}//namespace

LeleObject::LeleObject(LeleObject *parent, const std::string &json_str)
  : _lele_parent(parent), 
  _class_name(__func__ ) {
  fromJson(json_str);
  // std::cout << "styles:" << _lele_styles << "\n";
}

LeleObject::~LeleObject() {
    // lv_style_reset(&_style);
}

bool LeleObject::fromJson(const std::string &json_str) {

  auto json_tokens = LeleWidgetFactory::tokenizeJson(json_str);
  _lele_styles = LeleWidgetFactory::stylesFromJson(this, json_tokens);

  _nodes = LeleWidgetFactory::fromJson(this, json_tokens);
  for (const auto &[key, token]: _nodes) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "id") {
        _id = value;
      }
      else if(key == "enabled") {
        _enabled = (value == "true");
      }
    }
  }
  return true;
}

const std::string &LeleObject::className() const {
  return _class_name;
}
const std::string &LeleObject::id() const {
  return _id;
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
std::optional<LeleStyle::StyleValue> LeleObject::getStyle(const std::string &key, const std::string &class_name) const {

  for(const auto &lele_style : std::ranges::views::reverse(_lele_styles)) {
    auto value = lele_style->getValue(key, class_name.empty() ? lele_style->getClassName() : class_name);
    if(value) {
      return value;
    }
  }
  return std::optional<LeleStyle::StyleValue>();
}

namespace {
void setTextAlign(lv_obj_t *lv_obj, lv_text_align_t align_type) {

  const lv_obj_class_t *child_class = lv_obj_get_class(lv_obj);
  if(strcmp((char*)child_class->name, "label")==0) {
    lv_obj_set_style_text_align(lv_obj, align_type, 0);
  }
}

}//namespace

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

std::tuple<std::vector<std::string> ,std::map<std::string, std::optional<LeleStyle::StyleValue>>> LeleObject::getBackgroundStyle(const std::string &class_name) const {

  std::vector<std::string> bg_keys;
  std::map<std::string, std::optional<LeleStyle::StyleValue>> bg_style;
  for(const auto &lele_style : std::ranges::views::reverse(_lele_styles)) {
    for(const std::string &key : lele_style->getBackgroundAttributes()) {
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

void LeleObject::setStyle(lv_obj_t *lv_obj) {
  lv_style_init(&_style);

  auto value = getStyle("x");
  if(value) {
    lv_obj_set_x(lv_obj, std::get<int>(value.value()));
  }
  value = getStyle("y");
  if(value) {
    lv_obj_set_y(lv_obj, std::get<int>(value.value()));
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

  value = getStyle("corner_radius");
  if(value) {
    lv_style_set_radius(&_style, std::get<int>(value.value()));
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
  if(!value || std::get<LeleStyle::BorderTypeE>(value.value()) == LeleStyle::None) {
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

  value =getStyle("fgcolor");
  if(value) {
    lv_obj_set_style_text_color(lv_obj, lv_color_hex(std::get<int>(value.value())), LV_PART_MAIN);
  }
  value =getStyle("bgcolor");
  if(value) {
    int bgcolor = std::get<int>(value.value());
    lv_obj_set_style_bg_color(lv_obj, lv_color_hex(std::get<int>(value.value())), LV_PART_MAIN);
  }

  value = getStyle("layout");
  if(value) {
    lv_obj_set_style_layout(lv_obj, 
      std::get<lv_layout_t>(value.value()), //LV_LAYOUT_FLEX or LV_LAYOUT_GRID or LV_LAYOUT_NONE
      LV_STYLE_STATE_CMP_SAME);
  }
  value = getStyle("flow");
  if(value) {
    lv_obj_set_style_flex_flow(lv_obj, 
      std::get<lv_flex_flow_t>(value.value()), //LV_FLEX_FLOW_ROW or LV_FLEX_FLOW_COLUMN or ...
      LV_STYLE_STATE_CMP_SAME);

    value = getStyle("grow");
    if(value) {
      lv_obj_set_style_flex_grow(lv_obj, std::get<int>(value.value()), LV_STYLE_STATE_CMP_SAME);
    }
  }
  value = getStyle("align");
  if(value) {
    lv_align_t align = static_cast<lv_align_t>(std::get<int>(value.value()));
    lv_obj_align(lv_obj, 
      static_cast<lv_align_t>(std::get<int>(value.value())), 0, 0);
  }
  value = getStyle("text_align");
  if(value) {
    lv_text_align_t align_type = static_cast<lv_text_align_t>(std::get<int>(value.value()));
    lv_obj_set_style_text_align(lv_obj, align_type, 0);
  }

  value = getStyle("background/color");
  if(value) {
    if(!_bg_color) {
      _bg_color = LeleImageConverter::generateImgDsc(obj_width, obj_height, 3);
    }
    if(!_bg_color) {
      LL(FATAL, LVSIM) << "Failed to generate image for background color";
    }
    LeleImageConverter::fillImgDsc(_bg_color->get(), 
      std::get<int>(value.value())
    );
    _lv_bg_color = lv_image_create(lv_obj);
    lv_image_set_src(_lv_bg_color, _bg_color.value().get());
  }
  value = getStyle("background/image");
  if(value) {
    drawBackgroundImage(lv_obj, value, obj_width, obj_height);
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

void LeleObject::drawBackgroundImage(lv_obj_t *lv_obj, std::optional<LeleStyle::StyleValue> value, int obj_width, int obj_height) {

    std::string src = std::get<std::string>(value.value());
    if(src.at(0) == '/') {
      _bg_img = LeleImageConverter::generateImgDsc(src.c_str());
    }
    else {
      std::string img_path(std::filesystem::current_path().string() + "/" + src);
      if(!std::filesystem::exists(img_path)) {
        LOG(FATAL, LVSIM, "File does not exist: '%s'\n", img_path.c_str());
      }
      LOG(DEBUG, LVSIM, "Loading image: %s\n", img_path.c_str());
      _bg_img = LeleImageConverter::generateImgDsc(img_path.c_str());
    }
    if(!_bg_img) {
        LOG(FATAL, LVSIM, "Failed in generating image description");
        return;
    }
    struct XY {
      int _x = 0;
      int _y = 0;
    };
    XY offset;
    XY background_rotation_pivot;
    float background_rotation_angle;
    
    const auto &[keys, bg_style] = getBackgroundStyle();
    for(const auto &key: keys) {
      const auto &value = bg_style.at(key);
      if(key == "background/size") {
        std::string val = std::get<std::string>(value.value());
        if(val == "cover") {
          _bg_img = resizeContentToFillContainerPotentiallyCroppingContent(_bg_img.value().get(), obj_width, obj_height);
        }
        else if(val == "contain") {
          _bg_img = resizeToShowEntireContentPotentiallyLeavingEmptySpace(_bg_img.value().get(), obj_width, obj_height);
        }
        else if(!val.empty()) {
          _bg_img = resizeImageWithValuesParsedFromJson(_bg_img.value().get(), val, obj_width, obj_height);
        }
        if(!_bg_img) {
          LOG(FATAL, LVSIM, "Failed in processing background/size");
          return;
        }
      }
      else if(key == "background/position/x") {
        std::tie(offset._x, offset._y) = parseBackgroundPosition(value, obj_width, obj_height);
      }
      else if(key == "background/rotation/pivot/x") {
        // background_rotation_pivot._x = std::stoi(std::get<std::string>(value.value()));
        background_rotation_pivot._x = std::get<int>(value.value());
      }      
      else if(key == "background/rotation/pivot/y") {
        // background_rotation_pivot._y = std::stoi(std::get<std::string>(value.value()));
        background_rotation_pivot._y = std::get<int>(value.value());
      }      
      else if(key == "background/rotation/angle") {
        background_rotation_angle = std::get<float>(value.value());
        _bg_img = LeleImageConverter::rotateImg(_bg_img.value().get(), background_rotation_pivot._x, background_rotation_pivot._y, background_rotation_angle);
        if(!_bg_img) {
          LOG(FATAL, LVSIM, "Failed in processing background/rotate");
          return;
        }
        // std::string filename("rotated.");
        // filename += std::to_string(background_rotation_angle);
        // filename += ".png";
        // LeleImageConverter::saveGdImage(filename.c_str(), _bg_img.value().get());
      }
      else if(key == "background/repeat") {
        std::string val = std::get<std::string>(value.value());
        if(val == "repeat-x"){
          _bg_img = LeleImageConverter::tileImg(_bg_img.value().get(), obj_width, obj_height, LeleImageConverter::TileRepeat::RepeatX, offset._x, offset._y);
        }
        else if(val == "repeat-y"){
          _bg_img = LeleImageConverter::tileImg(_bg_img.value().get(), obj_width, obj_height, LeleImageConverter::TileRepeat::RepeatY, offset._x, offset._y);
        }
        else if(val == "repeat"){
          _bg_img = LeleImageConverter::tileImg(_bg_img.value().get(), obj_width, obj_height, LeleImageConverter::TileRepeat::RepeatXY, offset._x, offset._y);
        }
        if(!_bg_img) {
          LOG(FATAL, LVSIM, "Failed in background/repeat");
          return;
        }
      }
    }
    if(_bg_img.value().get()->header.w > obj_width || 
        _bg_img.value().get()->header.h > obj_height) {
        _bg_img = LeleImageConverter::cropImg(_bg_img.value().get(), 0, 0, obj_width, obj_height);
    }
    if(!_bg_img) {
      LOG(FATAL, LVSIM, "Failed in cropping image");
      return;
    }

    // LOG(DEBUG, LVSIM, "obj_width:%i, obj_height:%i\n", obj_width, obj_height);
    if(!_lv_bg_img) {
      _lv_bg_img = lv_image_create(_lv_bg_color ? _lv_bg_color : _lv_obj);
    }
    if(!_lv_bg_img) {
        LOG(FATAL, LVSIM, "Failed in lv_image_create");
        return;
    }
    lv_image_set_src(_lv_bg_img, _bg_img.value().get());
}

std::tuple<int,int> LeleObject::parseBackgroundPosition(
  const std::optional<LeleStyle::StyleValue> &value, int container_width, int container_height) const {
  int x = 0;
  int y = 0;
  std::string val = std::get<std::string>(value.value());
  if(!val.empty()) {
    val = LeleWidgetFactory::trim(val);
    if(!LeleWidgetFactory::parsePercentValues(
      val, 
      {{"x", &x}, {"y", &y}}, 
      {{"x", container_width}, {"y", container_height}})) {
      x = LeleStyle::parsePercentValue(val, container_width);
      y = LeleStyle::parsePercentValue(val, container_height);
    }
  }
  return std::tuple<int,int>(x, y);
}

void LeleObject::addStyle(std::vector<std::unique_ptr<LeleStyle>> &lele_styles) {
  if(lele_styles.size()){
    for(std::unique_ptr<LeleStyle> &lele_style : lele_styles) {
      _lele_styles.emplace_back(lele_style.get());
    }
    setStyle(_lv_obj);
  }
}

void LeleObject::addStyle(LeleStyle* lele_style) {
  _lele_styles.emplace_back(lele_style);
  setStyle(_lv_obj);
}

void LeleObject::removeStyle(const std::string &style_id) {
  //osm todo
}

void LeleObject::setObjAlignStyle(lv_obj_t *lv_obj) {
  auto value = getStyle("align");
  if(value) {
    lv_obj_align(lv_obj, 
      static_cast<lv_align_t>(
        std::get<int>(value.value())),
      0, 0);
    //make sure to do lv_obj_set_layout(parent, LV_LAYOUT_NONE); otherwise this function might not work
  }
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

void LeleObject::setTextAlignStyle(lv_obj_t *lv_obj) {

  auto value = getStyle("text_align");
  if(value) {
    lv_obj_set_style_text_align(lv_obj, 
      static_cast<lv_text_align_t>(
        std::get<int>(value.value())),
      // LV_PART_ANY);
      LV_PART_MAIN);
  }
}

lv_obj_t *LeleObject::createLvObj(LeleObject *lele_parent, lv_obj_t *lv_obj) {

  if(!lv_obj) {
    _lv_obj = lv_obj_create(lele_parent->getLvObj());
  }
  else {
    _lv_obj = lv_obj;
  }
  setParent(lele_parent);
  setStyle(_lv_obj);
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
