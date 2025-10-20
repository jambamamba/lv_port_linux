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
      else if(key == "rotation") {
        _rotation = parseRotation(value);
      }
      else if(key == "offset") {
        _offset = std::optional<LeleImage::Offset>();
        LeleWidgetFactory::parseXY(value, {{"x", &_offset->_offset_x}, {"y", &_offset->_offset_y}});
      }
      else if(key == "scale") {
        _scale = std::optional<LeleImage::Scale>();
        LeleWidgetFactory::parseXY(value, {{"x", &_scale->_percent_x}, {"y", &_scale->_percent_y}});
      }
      else if(key == "blendmode") {
        if(value == "additive")         { _blendmode = std::optional<lv_blend_mode_t>(LV_BLEND_MODE_ADDITIVE); }
        else if(value == "subtractive") { _blendmode = std::optional<lv_blend_mode_t>(LV_BLEND_MODE_SUBTRACTIVE); }
        else if(value == "multiply")    { _blendmode = std::optional<lv_blend_mode_t>(LV_BLEND_MODE_MULTIPLY); }
        else if(value == "difference")  { _blendmode = std::optional<lv_blend_mode_t>(LV_BLEND_MODE_DIFFERENCE); }
        else                            { _blendmode = std::optional<lv_blend_mode_t>(LV_BLEND_MODE_NORMAL); }
      }
      else if(key == "align") {
        if(value == "top_left" )            {_align = std::optional<lv_image_align_t>(LV_IMAGE_ALIGN_TOP_LEFT);}
        else if(value == "top_mid" )        {_align = std::optional<lv_image_align_t>(LV_IMAGE_ALIGN_TOP_MID);}
        else if(value == "top_right" )      {_align = std::optional<lv_image_align_t>(LV_IMAGE_ALIGN_TOP_RIGHT);}
        else if(value == "bottom_left" )    {_align = std::optional<lv_image_align_t>(LV_IMAGE_ALIGN_BOTTOM_LEFT);}
        else if(value == "bottom_mid" )     {_align = std::optional<lv_image_align_t>(LV_IMAGE_ALIGN_BOTTOM_MID);}
        else if(value == "bottom_right")    {_align = std::optional<lv_image_align_t>(LV_IMAGE_ALIGN_BOTTOM_RIGHT);}
        else if(value == "left_mid" )       {_align = std::optional<lv_image_align_t>(LV_IMAGE_ALIGN_LEFT_MID);}
        else if(value == "right_mid" )      {_align = std::optional<lv_image_align_t>(LV_IMAGE_ALIGN_RIGHT_MID);}
        else if(value == "center" )         {_align = std::optional<lv_image_align_t>(LV_IMAGE_ALIGN_CENTER);}
        else if(value == "auto_transform" ) {_align = std::optional<lv_image_align_t>(LV_IMAGE_ALIGN_AUTO_TRANSFORM);}
        else if(value == "stretch" )        {_align = std::optional<lv_image_align_t>(LV_IMAGE_ALIGN_STRETCH);}//Set X and Y scale to fill the Widget's area
        else if(value == "tile" )           {_align = std::optional<lv_image_align_t>(LV_IMAGE_ALIGN_TILE);}//Tile image to fill Widget's area. Offset is applied to shift the tiling
        else if(value == "contain" )        {_align = std::optional<lv_image_align_t>(LV_IMAGE_ALIGN_CONTAIN);}//The image keeps its aspect ratio, but is resized to the maximum size that fits within the Widget's area
        else if(value == "cover" )          {_align = std::optional<lv_image_align_t>(LV_IMAGE_ALIGN_COVER);}//The image keeps its aspect ratio and fills the Widget's area
        else                                {_align = std::optional<lv_image_align_t>(LV_IMAGE_ALIGN_DEFAULT);}
      }
      else if(key == "antialias") {
        _antialias = (value == "false") ? false:true;
      }
    }
  }
}

lv_obj_t *LeleImage::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {

  _lv_obj = LeleBase::createLvObj(lele_parent,
    lv_image_create(lele_parent->getLvObj()));

  std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> img;
  if(!_src.empty()) {
    if(_src.at(0) == '/') {
      img = LeleImageConverter::generateImgDsc(_src.c_str());
    }
    else {
      std::string img_path(applicationPath().parent_path().string() + "/res/" + _src);
      img = LeleImageConverter::generateImgDsc(img_path.c_str());
    }
  }
  if(!img) {
    if(!_src.empty()) {
      LOG(WARNING, LVSIM, "LeleImage::createLvObj FAILED to load image: '%s'\n", _src.c_str()); 
    }
    return _lv_obj;
  }

  lv_image_set_src(_lv_obj, img.value().get());
  _images[_src] = img;

  int width = lv_obj_get_width(_lv_obj);
  int height = lv_obj_get_height(_lv_obj);
  LOG(DEBUG, LVSIM, "@@@ LeleImage::createLvObj _lv_obj w:%i, h:%i\n", width, height);
  lv_image_t *lv_image = reinterpret_cast<lv_image_t *>(_lv_obj);
  LOG(DEBUG, LVSIM, "@@@ LeleImage::createLvObj lv_image w:%i, h:%i, sx:%i, sy:%i\n", lv_image->w, lv_image->h, lv_image->scale_x, lv_image->scale_y);
  lv_image_dsc_t *lv_image_dsc = img.value().get();
  LOG(DEBUG, LVSIM, "@@@ LeleImage::createLvObj lv_image_dsc w:%i, h:%i\n", lv_image_dsc->header.w, lv_image_dsc->header.h);

  if(_blendmode) {
    lv_image_set_blend_mode(_lv_obj, _blendmode.value());
  }
  if(_antialias) {
    lv_image_set_antialias(_lv_obj, _antialias.value());
  }
  if(_rotation) {
    lv_image_set_pivot(_lv_obj, _rotation->_pivot_x, _rotation->_pivot_y);
    lv_image_set_rotation(_lv_obj, _rotation->_angle);
  }
  if(_offset) {
    lv_image_set_offset_x(_lv_obj, _offset->_offset_x);
    lv_image_set_offset_y(_lv_obj, _offset->_offset_y);
  }
  if(_scale) {
    if(_scale->_percent_x == _scale->_percent_y) {
      lv_image_set_scale(_lv_obj, LV_SCALE_NONE * _scale->_percent_x / 100);
    }
    else {
      lv_image_set_scale_x(_lv_obj, LV_SCALE_NONE * _scale->_percent_x / 100);
      lv_image_set_scale_y(_lv_obj, LV_SCALE_NONE * _scale->_percent_y / 100);
    }
  }
  if(_align) {
    lv_image_set_inner_align(_lv_obj, _align.value());
  }

  return _lv_obj;
}

std::optional<LeleImage::Rotation> LeleImage::parseRotation(const std::string &json_str) {
  bool processed = false;
  LeleImage::Rotation rotation;
  for (const auto &[key, token]: LeleWidgetFactory::fromJson(json_str)) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "angle") {
        rotation._angle = std::stof(value);
        processed = true;
      }
      else if(key == "pivot") {
        LeleWidgetFactory::parseXY(value, {{"x", &rotation._pivot_x}, {"y", &rotation._pivot_y}});
        processed = true;
      }
    }
  }
  return processed ? std::optional<LeleImage::Rotation>(rotation) : std::nullopt;
}

std::string LeleImage::getSrc() const { 
  return _src; 
}
