#include "leleimage.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleImage::LeleImage(const std::string &json_str)
  : LeleBase(json_str) {

  _class_name = __func__ ;//typeid(this).name();
  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "src") {
        _src = value;
      }
      else if(key == "name") {
        _name = value;
      }
      else if(key == "x") {
        _name = std::stoi(value);
      }
      else if(key == "y") {
        _name = std::stoi(value);
      }
      else if(key == "rotation") {
        parseRotation(value);
      }
      else if(key == "scale_percent") {
        LeleWidgetFactory::fromJson(value, [this](const std::string &key, int value){
          if(key.empty()) {
            _scale._percent_x = _scale._percent_x = value;
          }
          else if(key == "x") {
            _scale._percent_x = value;
          }
          else if(key == "y") {
            _scale._percent_y = value;
          }
        });
      }
      else if(key == "blendmode") {
        if(value == "additive") { _blendmode = LV_BLEND_MODE_ADDITIVE; }
        else if(value == "subtractive") { _blendmode = LV_BLEND_MODE_SUBTRACTIVE; }
        else if(value == "multiply") { _blendmode = LV_BLEND_MODE_MULTIPLY; }
        else if(value == "difference") { _blendmode = LV_BLEND_MODE_DIFFERENCE; }
        else { _blendmode = LV_BLEND_MODE_NORMAL; }
      }
      else if(key == "align") {
        if(value == "top_left" ) {_align = LV_IMAGE_ALIGN_TOP_LEFT;}
        else if(value == "top_mid" ) {_align = LV_IMAGE_ALIGN_TOP_MID;}
        else if(value == "top_right" ) {_align = LV_IMAGE_ALIGN_TOP_RIGHT;}
        else if(value == "bottom_left" ) {_align = LV_IMAGE_ALIGN_BOTTOM_LEFT;}
        else if(value == "bottom_mid" ) {_align = LV_IMAGE_ALIGN_BOTTOM_MID;}
        else if(value == "bottom_right" ) {_align = LV_IMAGE_ALIGN_BOTTOM_RIGHT;}
        else if(value == "left_mid" ) {_align = LV_IMAGE_ALIGN_LEFT_MID;}
        else if(value == "right_mid" ) {_align = LV_IMAGE_ALIGN_RIGHT_MID;}
        else if(value == "center" ) {_align = LV_IMAGE_ALIGN_CENTER;}
        else if(value == "auto_transform" ) {_align = LV_IMAGE_ALIGN_AUTO_TRANSFORM;}
        else if(value == "stretch" ) {_align = LV_IMAGE_ALIGN_STRETCH;}//Set X and Y scale to fill the Widget's area.
        else if(value == "tile" ) {_align = LV_IMAGE_ALIGN_TILE;}//Tile image to fill Widget's area. Offset is applied to shift the tiling.
        else if(value == "contain" ) {_align = LV_IMAGE_ALIGN_CONTAIN;}//The image keeps its aspect ratio, but is resized to the maximum size that fits within the Widget's area.
        else if(value == "cover" ) {_align = LV_IMAGE_ALIGN_COVER;}//The image keeps its aspect ratio and fills the Widget's area.
        else {_align = LV_IMAGE_ALIGN_DEFAULT;}
      }
      else if(key == "antialias") {
        _antialias = (value == "false") ? false:true;
      }
    }
  }
}

void LeleImage::parseRotation(const std::string &json_str) {
  for (const auto &[key, token]: LeleWidgetFactory::fromJson(json_str)) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "angle") { 
        _rotation._angle = std::stof(value); 
      }
      else if(key == "pivot") {
        LeleWidgetFactory::fromJson(value, [this](const std::string &key, int value){
          if(key.empty()) {
            _rotation._pivot_x = _rotation._pivot_y = value;
          }
          else if(key == "x") {
            _rotation._pivot_x = value;
          }
          else if(key == "y") {
            _rotation._pivot_y = value;
          }
        });
      }
    }
  }
}

lv_obj_t *LeleImage::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {

  _lv_obj = LeleBase::createLvObj(lele_parent,
    lv_image_create(lele_parent->getLvObj()));

  setSrc(_src);

  // osm todo: value should be these, see (/repos/lv_port_linux/lvgl/src/widgets/image/lv_image.h)
  // {
  //   "src":"/path/to/image/file", 
  //   "name":"image_name",
  //   "x":"0",
  //   "y":"0",
  //   "rotation/angle":"0",
  //   "rotation/pivot/x":"0",
  //   "rotation/pivot/y":"0",
  //   "scale/percent":"100",
  //   "scale/percent/x":"100",
  //   "scale/percent/y":"100",
  //   "blendmode":"?",
  //   "anti-aliasing":"true",
  //   "align":"",//LV_IMAGE_ALIGN_STRETCH | LV_IMAGE_ALIGN_FIT
  // }

  return _lv_obj;
}

void LeleImage::setSrc(const std::string &value) {
  if(!value.empty()) {
    if(value.at(0) == '/') {
    _images[value] = generateImgDsc(value.c_str());
    }
    else {
      std::string img_path(applicationPath().parent_path().string() + "/res/" + value);
      _images[value] = generateImgDsc(img_path.c_str());
    }
    lv_image_set_src(_lv_obj, _images[value].value().get());
    _src = value;
  }
}

std::string LeleImage::getSrc() const { 
  return _src; 
}
