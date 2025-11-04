#include <iostream>
#include <lvgl/lvgl_private.h>
#include </repos/lv_port_linux/lvgl/src/core/lv_obj_tree.h>

#include "lelebase.h"


LOG_CATEGORY(LVSIM, "LVSIM");

namespace {
std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> resizeImageWithValuesParsedFromJson(
  const lv_image_dsc_t *src_img, std::string val, int container_width, int container_height) {

  int x = -1;
  int y = -1;
  val = LeleWidgetFactory::trim(val);
  if(LeleWidgetFactory::parsePercentValues(val, {{"x", &x}, {"y", &y}}, {{"x", container_width}, {"y", container_height}})) {
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

LeleBase::LeleBase(const std::string &json_str)
: _class_name(__func__ ) {

  _tokens = LeleWidgetFactory::fromJson(json_str);
  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::unique_ptr<LeleStyle>>(token)) {
      if(key == "style") {
        auto &value = std::get<std::unique_ptr<LeleStyle>>(token);
        LeleStyle *lele_style = dynamic_cast<LeleStyle*> (value.get());
        _lele_styles += *lele_style;
      }
    }
    else if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "id") {
        _id = value;
      }
      else if(key == "enabled") {
        _enabled = (value == "true");
      }
    }
  }
  // std::cout << "styles:" << _lele_styles << "\n";
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

void LeleBase::setStyle(lv_obj_t *lv_obj) {
  lv_style_init(&_style);
  auto value = _lele_styles.getValue("corner_radius");
  if(value) {
    lv_style_set_radius(&_style, std::get<int>(value.value()));
  }
  int obj_width = -1;
  value = _lele_styles.getValue("width");
  if(value) {
    obj_width = std::get<int>(value.value());
    lv_style_set_width(&_style, obj_width);
    // lv_obj_set_width(obj, lv_pct(100));//to set an object's size as a percentage of its parent's size
    // LV_SIZE_CONTENT //to make an object automatically size itself to fit its children
  }
  int obj_height = -1;
  value = _lele_styles.getValue("height");
  if(value) {
    obj_height = std::get<int>(value.value());
    lv_style_set_height(&_style, obj_height);
    // lv_obj_set_height(obj, lv_pct(100));//to set an object's size as a percentage of its parent's size
    // LV_SIZE_CONTENT //to make an object automatically size itself to fit its children
  }
  value = _lele_styles.getValue("x");
  if(value) {
    lv_style_set_x(&_style, lv_pct(std::get<int>(value.value())));
  }
  value = _lele_styles.getValue("y");
  if(value) {
    lv_style_set_y(&_style, lv_pct(std::get<int>(value.value())));
  }

  value = _lele_styles.getValue("padding/top");
  if(value) {
    lv_style_set_pad_top(&_style, std::get<int>(value.value()));
  }
  value = _lele_styles.getValue("padding/right");
  if(value) {
    lv_style_set_pad_right(&_style, std::get<int>(value.value()));
  }
  value = _lele_styles.getValue("padding/bottom");
  if(value) {
    lv_style_set_pad_bottom(&_style, std::get<int>(value.value()));
  }
  value = _lele_styles.getValue("padding/left");
  if(value) {
    lv_style_set_pad_left(&_style, std::get<int>(value.value()));
  }

  value = _lele_styles.getValue("margin/top");
  if(value) {
    lv_style_set_margin_top(&_style, std::get<int>(value.value()));
  }
  value = _lele_styles.getValue("margin/right");
  if(value) {
    lv_style_set_margin_right(&_style, std::get<int>(value.value()));
  }
  value = _lele_styles.getValue("margin/bottom");
  if(value) {
    lv_style_set_margin_bottom(&_style, std::get<int>(value.value()));
  }
  value = _lele_styles.getValue("margin/left");
  if(value) {
    lv_style_set_margin_left(&_style, std::get<int>(value.value()));
  }

  value = _lele_styles.getValue("border/type");
  if(!value || std::get<LeleStyle::BorderTypeE>(value.value()) == LeleStyle::None) {
    lv_style_set_border_width(&_style, 0);
  }
  else {
    value = _lele_styles.getValue("border/color");
    if(value) {
      lv_style_set_border_color(&_style, lv_color_hex(std::get<int>(value.value())));
    }
    value = _lele_styles.getValue("border/width");
    if(value) {
      lv_style_set_border_width(&_style, std::get<int>(value.value()));
    }
  }

  value = _lele_styles.getValue("x");
  if(value) {
    lv_obj_set_x(lv_obj, std::get<int>(value.value()));
  }
  value = _lele_styles.getValue("y");
  if(value) {
    lv_obj_set_y(lv_obj, std::get<int>(value.value()));
  }
  value = _lele_styles.getValue("width");
  if(value) {
    obj_width = std::get<int>(value.value());
    lv_obj_set_width(lv_obj, obj_width);
  }
  value = _lele_styles.getValue("height");
  if(value) {
    obj_height = std::get<int>(value.value());
    lv_obj_set_height(lv_obj, obj_height);
  }

  value =_lele_styles.getValue("fgcolor");
  if(value) {
    lv_obj_set_style_text_color(lv_obj, lv_color_hex(std::get<int>(value.value())), LV_PART_MAIN);
  }
  value =_lele_styles.getValue("bgcolor");
  if(value) {
    lv_obj_set_style_bg_color(lv_obj, lv_color_hex(std::get<int>(value.value())), LV_PART_MAIN);
  }

  value = _lele_styles.getValue("layout");
  if(value) {
    lv_obj_set_style_layout(lv_obj, 
      std::get<lv_layout_t>(value.value()), //LV_LAYOUT_FLEX or LV_LAYOUT_GRID or LV_LAYOUT_NONE
      LV_STYLE_STATE_CMP_SAME);
  }
  value = _lele_styles.getValue("flow");
  if(value) {
    lv_obj_set_style_flex_flow(lv_obj, 
      std::get<lv_flex_flow_t>(value.value()), //LV_FLEX_FLOW_ROW or LV_FLEX_FLOW_COLUMN or ...
      LV_STYLE_STATE_CMP_SAME);

    value = _lele_styles.getValue("grow");
    if(value) {
      lv_obj_set_style_flex_grow(lv_obj, std::get<int>(value.value()), LV_STYLE_STATE_CMP_SAME);
    }
  }

  value = _lele_styles.getValue("align");
  if(value) {
    lv_obj_align(lv_obj, 
      static_cast<lv_align_t>(std::get<int>(value.value())), 0, 0);
  }
  value = _lele_styles.getValue("text_align");
  if(value) {
    lv_obj_set_style_text_align(lv_obj, 
      static_cast<lv_text_align_t>(std::get<int>(value.value())), 0);
  }

  value = _lele_styles.getValue("background/color");
  if(value) {
    lv_obj_set_style_bg_color(lv_obj, lv_color_hex(std::get<int>(value.value())), LV_PART_MAIN);
  }
  value = _lele_styles.getValue("background/image");
  if(value) {
    drawBackgroundImage(value, obj_width, obj_height);
  }
  value = _lele_styles.getValue("scrollbar");
  if(value) {
    lv_obj_set_scrollbar_mode(lv_obj, std::get<lv_scrollbar_mode_t>(value.value()));
  }
  
  // lv_theme_t * my_theme = lv_theme_create_from_default(lv_disp_get_default(), lv_color_hex(0x0000FF), lv_color_hex(0x00FF00)); // Create a new theme
  // lv_theme_set_active(my_theme); // Set the new theme as active

  // lv_style_t * btn_style = lv_theme_get_style(my_theme, LV_PART_MAIN); // Get the button style from your custom theme
  // lv_style_set_bg_color(btn_style, lv_color_hex(0x0000FF), LV_STATE_DEFAULT); // Customize the button style
  lv_obj_add_style(lv_obj, &_style, LV_PART_MAIN);
}

void LeleBase::drawBackgroundImage(std::optional<LeleStyle::StyleValue> value, int obj_width, int obj_height) {
    lv_obj_t *lv_img = lv_image_create(_lv_obj);
    if(!lv_img) {
        LOG(FATAL, LVSIM, "Failed in lv_image_create");
        return;
    }
    std::string src = std::get<std::string>(value.value());
    if(src.at(0) == '/') {
      _bg_img = LeleImageConverter::generateImgDsc(src.c_str());
    }
    else {
      std::string img_path(applicationPath().parent_path().string() + "/res/" + src);
      _bg_img = LeleImageConverter::generateImgDsc(img_path.c_str());
    }
    if(!_bg_img) {
        LOG(FATAL, LVSIM, "Failed in generating image description");
        return;
    }
    value = _lele_styles.getValue("background/size");
    if(value) {
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
    }
    if(!_bg_img) {
      LOG(FATAL, LVSIM, "Failed in processing background/size");
      return;
    }
    int offset_x = 0;
    int offset_y = 0;
    value = _lele_styles.getValue("background/position");
    if(value) {
      std::tie(offset_x, offset_y) = parseBackgroundPosition(value, obj_width, obj_height);
    }
    value = _lele_styles.getValue("background/repeat");
    if(value) {
      std::string val = std::get<std::string>(value.value());
      if(val == "repeat-x"){
        _bg_img = LeleImageConverter::tileImg(_bg_img.value().get(), obj_width, obj_height, LeleImageConverter::TileRepeat::RepeatX, offset_x, offset_y);
      }
      else if(val == "repeat-y"){
        _bg_img = LeleImageConverter::tileImg(_bg_img.value().get(), obj_width, obj_height, LeleImageConverter::TileRepeat::RepeatY, offset_x, offset_y);
      }
      else if(val == "repeat"){
        _bg_img = LeleImageConverter::tileImg(_bg_img.value().get(), obj_width, obj_height, LeleImageConverter::TileRepeat::RepeatXY, offset_x, offset_y);
      }
    }
    if(!_bg_img) {
      LOG(FATAL, LVSIM, "Failed in background/repeat");
      return;
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
    lv_image_set_src(lv_img, _bg_img.value().get());
}

std::tuple<int,int> LeleBase::parseBackgroundPosition(
  const std::optional<LeleStyle::StyleValue> &value, int container_width, int container_height) const {
  int x = 0;
  int y = 0;
  std::string val = std::get<std::string>(value.value());
  if(!val.empty()) {
    val = LeleWidgetFactory::trim(val);
    if(!LeleWidgetFactory::parsePercentValues(val, {{"x", &x}, {"y", &y}}, {{"x", container_width}, {"y", container_height}})) {
      x = LeleStyle::parsePercentValue(val, container_width);
      y = LeleStyle::parsePercentValue(val, container_height);
    }
  }
  return std::tuple<int,int>(x, y);
}
void LeleBase::setObjAlignStyle(lv_obj_t *lv_obj) {
  auto value = _lele_styles.getValue("align");
  if(value) {
    lv_obj_align(lv_obj, 
      static_cast<lv_align_t>(
        std::get<int>(value.value())),
      0, 0);
    //make sure to do lv_obj_set_layout(parent, LV_LAYOUT_NONE); otherwise this function might not work
  }
}

void LeleBase::setTextAlignStyle(lv_obj_t *lv_obj) {

  auto value = _lele_styles.getValue("text_align");
  if(value) {
    lv_obj_set_style_text_align(lv_obj, 
      static_cast<lv_text_align_t>(
        std::get<int>(value.value())),
      LV_PART_MAIN);
  }
}

lv_obj_t *LeleBase::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {

  if(!lv_obj) {
    _lv_obj = lv_obj_create(lele_parent->getLvObj());
  }
  else {
    _lv_obj = lv_obj;
  }
  setParent(lele_parent);
  setStyle(_lv_obj);
  return _lv_obj;
}

std::vector<LeleBase *> LeleBase::getLeleObj(const std::string &obj_name) const {
  std::vector<LeleBase *> res;
  for(const auto &pair: _tokens) {
    if (std::holds_alternative<std::unique_ptr<LeleBase>>(pair.second)) {
      auto &value = std::get<std::unique_ptr<LeleBase>>(pair.second);
      if(pair.first == obj_name) {
        res.push_back(value.get());
      }
    }
  }
  return res;
}

void LeleBase::EventCallback(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    LeleBase *base = static_cast<LeleBase*>(e->user_data);
    if(base) {
      base->eventCallback(LeleEvent(e));
    }
}

void LeleBase::hide() {
  // int width = lv_obj_get_width(getLvObj());
  // int height = lv_obj_get_height(getLvObj());
  // lv_obj_set_size(getLvObj(), 0, 0);
  lv_obj_add_flag(getLvObj(), LV_OBJ_FLAG_HIDDEN);
}

void LeleBase::show() {
  lv_obj_remove_flag(getLvObj(), LV_OBJ_FLAG_HIDDEN);
}

bool LeleBase::eventCallback(LeleEvent &&e) {
  // LOG(DEBUG, LVSIM, "LeleBase::eventCallback id:%s, class_name:%s, _lele_parent:%s\n", 
    // _id.c_str(), _class_name.c_str(), _lele_parent->className().c_str());

  if(_lele_parent) {
    return _lele_parent->eventCallback(std::move(e));
  }
  return true;
}
std::ostream& operator<<(std::ostream& os, const LeleBase& p) {
    // os << "LeleStyles id: " << p._id << ", ";
    os << "_id:" << p._id << ",";
    os << "_class_name:" << p._class_name << ",";
    os << "\nStyles {\n";
    os << p._lele_styles;
    return os;
}
