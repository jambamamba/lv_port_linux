#include "leleimage.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleImage::LeleImage(const std::string &json_str)
  : LeleBase(json_str) {

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
  _class_name = __func__ ;//typeid(this).name();
  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "src") {
        _src = value;
      }
    }
  }
}

lv_obj_t *LeleImage::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {

  _lv_obj = LeleBase::createLvObj(lele_parent,
    lv_image_create(lele_parent->getLvObj()));

  setSrc(_src);
  return _lv_obj;
}

void LeleImage::setSrc(const std::string &value) {
  if(!value.empty()) {
    std::string img_path(applicationPath().parent_path().string() + "/res/" + value);
    _images[value] = generateImgDsc(img_path.c_str());
    lv_image_set_src(_lv_obj, _images[value].value().get());
    _src = value;
  }
}

std::string LeleImage::getSrc() const { 
  return _src; 
}
