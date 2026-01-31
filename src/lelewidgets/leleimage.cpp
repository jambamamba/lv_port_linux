#include "leleimage.h"

#include <image_builder/image_builder.h>

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

  _img_dsc = ImageBuilder::drawBackgroundImage(
    prefix,
    std::get<std::string>(_img_style["img/src"].value()),
    _attributes_as_ordered_in_json,
    _img_style,
    std::get<int>(width.value()),
    std::get<int>(height.value()));
  if(_img_dsc) {
    lv_image_set_src(_lv_img, _img_dsc.value().get());
  }
}

std::string LeleImage::getSrc() const { 
  auto src = _img_style.find("img/src");
  if(src == _img_style.end()) {
    return "";
  }
  if(!src->second) {
    return "";
  }
  std::string img_src = std::get<std::string>(src->second.value());
  return img_src;
}

void LeleImage::setSrc(const std::string& src) {
  _img_style["img/src"] = src;
  drawImage();
}

std::string LeleImage::getSize() const {
  return "";//osm todo
}
void LeleImage::setSize(const std::string& src) {
  //osm todo
}
std::string LeleImage::getPosition() const {
  return "";//osm todo
}
void LeleImage::setPosition(const std::string& src) {
  //osm todo
}
std::string LeleImage::getRotation() const {
  return "";//osm todo
}
void LeleImage::setRotation(const std::string& src) {
  //osm todo
}
