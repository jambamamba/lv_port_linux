#include "leleimage.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleImage::LeleImage(LeleObject *parent, const std::string &json_str)
  : LeleObject(parent, json_str) {

  _class_name = __func__ ;//typeid(this).name();
  fromJson(json_str);
}

namespace {
const std::string prefix("img");
}

bool LeleImage::fromJson(const std::string &json_str) {
  std::tie(_img_style, _attributes_as_ordered_in_json) = ImageBuilder::parseBackground("img", json_str, this);
  for(auto &item: _attributes_as_ordered_in_json) {
    item = prefix + "/" + item;
  }
  return true;
}

lv_obj_t *LeleImage::createLvObj(LeleObject *lele_parent, lv_obj_t *lv_obj_) {

  _lv_obj = LeleObject::createLvObj(lele_parent,
    lv_image_create(lele_parent->getLvObj()));
  
  _lv_img = lv_image_create(
      _lv_bg_img ? _lv_bg_img : 
        _lv_bg_color ? _lv_bg_color : 
          _lv_obj);

  drawImage();
  return _lv_obj;
}

void LeleImage::drawImage() {
  auto width = getStyle("width");
  auto height = getStyle("height");

  _img = ImageBuilder::drawBackgroundImage(
    prefix,
    std::get<std::string>(_img_style["img/src"].value()),
    _attributes_as_ordered_in_json,
    _img_style,
    std::get<int>(width.value()),
    std::get<int>(height.value()));
  if(_img._img_dsc) {
    lv_image_set_src(_lv_img, _img._img_dsc.value().get());
  }
  lv_obj_set_pos(_lv_img, _img._offset._x, _img._offset._y);
}

std::string LeleImage::getSrc() const { 
  auto src = _img_style.find("img/src");
  if(src == _img_style.end() || !src->second) {
    return "";
  }
  std::string img_src = std::get<std::string>(src->second.value());
  return img_src;
}

void LeleImage::setSrc(const std::string& src) {
  bool found = std::any_of(_attributes_as_ordered_in_json.begin(), _attributes_as_ordered_in_json.end(), [](const std::string& s) {
      return "img/src" == s;
  });
  if(!found) {
    _attributes_as_ordered_in_json.push_back("img/src");
  }

  _img_style["img/src"] = src;
  drawImage();
}

std::tuple<int,int> LeleImage::getSize() const {
  if(!_img._img_dsc){
    return {0,0};
  }
  int width = _img._img_dsc.value()->header.w;
  int height = _img._img_dsc.value()->header.h;
  return {width, height};
}
void LeleImage::setSize(int width, int height) {
  if(!_img._img_dsc){
    LL(WARNING, LVSIM) << "No image therefore cannot set width and height";
    return;
  }
  _img._img_dsc = LeleImageConverter::resizeImg(_img._img_dsc.value().get(), width, height);
  if(!_img._img_dsc) {
    LL(WARNING, LVSIM) << "Failed to resize image to width: " << width << ", hieght: " << height;
    return;
  }
  lv_image_set_src(_lv_img, _img._img_dsc.value().get());
}
std::tuple<int,int> LeleImage::getPosition() const {
  return {_img._offset._x, _img._offset._y};
}
void LeleImage::setPosition(int x, int y) {
  _img._offset._x = x;
  _img._offset._y = y;
  lv_obj_set_pos(_lv_img, _img._offset._x, _img._offset._y);
}
std::tuple<float, int,int> LeleImage::getRotation() const {
  auto angle = _img_style.find("img/rotation/angle");
  if(angle == _img_style.end() || !angle->second) {
    return {0.,0,0};
  }

  auto pivot_x = _img_style.find("img/rotation/pivot/x");
  if(pivot_x == _img_style.end() || !pivot_x->second) {
    return {0.,0,0};
  }

  auto pivot_y = _img_style.find("img/rotation/pivot/y");
  if(pivot_y == _img_style.end() || !pivot_y->second) {
    return {0.,0,0};
  }

  return {
    std::get<float>(angle->second.value()),
    std::get<int>(pivot_x->second.value()),
    std::get<int>(pivot_y->second.value()),
  };
}
void LeleImage::setRotation(float angle, int pivot_x, int pivot_y) {
  bool found = std::any_of(_attributes_as_ordered_in_json.begin(), _attributes_as_ordered_in_json.end(), [](const std::string& s) {
      return "img/rotation/angle" == s;
  });
  if(!found) {
    _attributes_as_ordered_in_json.push_back("img/rotation/angle");
  }

  found = std::any_of(_attributes_as_ordered_in_json.begin(), _attributes_as_ordered_in_json.end(), [](const std::string& s) {
      return "img/rotation/pivot/x" == s;
  });
  if(!found) {
    _attributes_as_ordered_in_json.push_back("img/rotation/pivot/x");
  }
  found = std::any_of(_attributes_as_ordered_in_json.begin(), _attributes_as_ordered_in_json.end(), [](const std::string& s) {
      return "img/rotation/pivot/y" == s;
  });
  if(!found) {
    _attributes_as_ordered_in_json.push_back("img/rotation/pivot/y");
  }

  _img_style["img/rotation/angle"] = angle;
  _img_style["img/rotation/pivot/x"] = pivot_x;
  _img_style["img/rotation/pivot/y"] = pivot_y;
  drawImage();
}
