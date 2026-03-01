#include "image_builder.h"

#include <debug_logger/debug_logger.h>
#include <lelewidgets/lelestyle.h>
#include <lelewidgets/leleobject.h>
#include <lelewidgets/lelewidgetfactory.h>
#include <lv_image_converter/lv_image_converter.h>
#include <ranges>

LOG_CATEGORY(LVSIM, "LVSIM");

namespace {
std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> resizeImageWithValuesParsedFromJson(
  const lv_image_dsc_t *src_img, std::string val, int container_width, int container_height) {

  int width = -1;
  int height = -1;
  val = LeleWidgetFactory::trim(val);
  if(LeleWidgetFactory::parsePercentValues(
    val, 
    {{"width", &width}, {"height", &height}}, 
    {{"width", container_width}, {"height", container_height}})) {
    return LeleImageConverter::resizeImg(src_img, width, height);
  }
  width = LeleStyle::parsePercentValue(val, container_width);
  height = LeleStyle::parsePercentValue(val, container_height);
  return LeleImageConverter::resizeImg(src_img, width, height);
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

std::tuple<int,int> parseImageJsonPosition(
  const std::optional<LeleStyle::StyleValue> &value, int container_width, int container_height) {
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

std::map<std::string, float> parseRotation(const std::string &json_str, LeleObject *lele_obj) {
  bool processed = false;
  int max_x = ImageBuilder::getParentDimension("width", lele_obj);
  int max_y = ImageBuilder::getParentDimension("height", lele_obj);
  std::map<std::string, float> res;
  for (const auto &[key, token]: LeleWidgetFactory::fromJson(lele_obj, json_str)) {
    if (!std::holds_alternative<std::string>(token)) {
      continue;
    }
    const std::string &value = std::get<std::string>(token);
    if(key == "angle") {
      res[key] = std::stof(value);
      processed = true;
    }
    else if(key == "pivot") {
      int pivot_x;
      int pivot_y;
      LeleWidgetFactory::parsePercentValues(
        value, 
        {{"x", &pivot_x}, {"y", &pivot_y}},
        {{"x", max_x}, {"y", max_y}}
      );
      res["pivot/x"] = pivot_x;
      res["pivot/y"] = pivot_y;
      processed = true;
    }
  }
  if(res.find("angle") != res.end()){
    if(res.find("pivot/x") == res.end() && res.find("pivot/y") == res.end()) {
      res["pivot/x"] = max_x/2;
      res["pivot/y"] = max_y/2;
    }
    else if(res.find("pivot/x") == res.end()) {
      res["pivot/x"] = max_x/2;
    }
    else if(res.find("pivot/y") == res.end()) {
      res["pivot/y"] = max_y/2;
    }
  }
  return processed ? res: std::map<std::string, float>();
}

}//namespace

int ImageBuilder::getParentDimension(const std::string &key, const LeleObject *lele_obj) {
  if(lele_obj && lele_obj->getParent()) {
    auto value = lele_obj->getParent()->getStyle(key);
    if(!value) {
      value = getParentDimension(key, lele_obj->getParent());
    }
    if(!value){
      LL(WARNING, LVSIM) << "style key:" << key << " has no value";
    }
    else if(std::holds_alternative<int>(value.value())) {
      int val = std::get<int>(value.value());
      return std::get<int>(value.value());
    }
    else {
      LL(WARNING, LVSIM) << "style key:" << key << " has non-int value";
    }
  }
  else if(lele_obj) {
    if(key == "x" || key == "width") { return lv_obj_get_width(lv_screen_active()); }
    else if(key == "y" || key == "height") { return lv_obj_get_height(lv_screen_active()); }
    else if(key == "corner-radius") { 
      return std::max(
        lv_obj_get_width(lv_screen_active()), 
        lv_obj_get_height(lv_screen_active())
      ); 
    }
    else {
      LL(WARNING, LVSIM) << "style key:" << key << " not handled";
    }
  }
  return 0;
}

int ImageBuilder::parseColorCode(const std::string &color_str) {
  if(color_str.empty()) {
    return 0;
  }
  else if(strcmp(color_str.c_str(), "red") == 0) {
    return 0xff0000;
  }
  else if(strcmp(color_str.c_str(), "green") == 0) {
    return 0x00ff00;
  }
  else if(strcmp(color_str.c_str(), "blue") == 0) {
    return 0x0000ff;
  }
  else if(strcmp(color_str.c_str(), "white") == 0) {
    return 0xffffff;
  }
  else if(strcmp(color_str.c_str(), "black") == 0) {
    return 0x000000;
  }
  else if(std::all_of(color_str.begin(), color_str.end(),
    [](unsigned char ch){ return std::isdigit(ch); })) {
    return std::stoi(color_str.c_str(), nullptr, 10);
  }
  else if(color_str.c_str()[0] == '#') {
    std::string suffix(color_str.c_str() + 1);
    if(std::all_of(suffix.begin(), suffix.end(),
      [](uint32_t ch){return ch >= '0' && ch <= ('0' + 0xff); })) {
          if(suffix.size() == 3) { // #fff => #ffffff
              std::string value;
              for(int i=0; i<3; ++i) {
                  value+=suffix.at(i);
                  value+=suffix.at(i);
              }
              return std::stoi(value.c_str(), nullptr, 16);
          }
          else {
            return std::stoi(suffix.c_str(), nullptr, 16);
          }
    }
  }
  return 0;
}

std::pair<std::map<std::string, std::optional<LeleStyle::StyleValue>>, std::vector<std::string>>
ImageBuilder::parseImageJson(const std::string &key, const std::string &value_, LeleObject *lele_obj) {

  std::map<std::string, std::optional<LeleStyle::StyleValue>> style;
  std::vector<std::string> attributes;
  LeleWidgetFactory::fromJson(value_, [&key, &style, &attributes, lele_obj](const std::string &subkey_, const std::string &value){
    std::string subkey(subkey_);
    if(subkey == "color") {
      style[key + "/" + subkey] = parseColorCode(value);
    }
    else if(subkey == "rotation") {
      auto rotation = parseRotation(value, lele_obj);
      if(!rotation.empty()) {
        style[key + "/rotation/pivot/x"] = static_cast<int>(rotation["pivot/x"]);
        style[key + "/rotation/pivot/y"] = static_cast<int>(rotation["pivot/y"]);
        style[key + "/rotation/angle"] = rotation["angle"];
        attributes.push_back("rotation/pivot/x");
        attributes.push_back("rotation/pivot/y");
        attributes.push_back("rotation/angle");
        return;
      }
    }
    else if(subkey == "image") {
      style[key + "/" + subkey] = value;
    }
    else if(subkey == "src") {
      style[key + "/" + subkey] = value;
    }
    else if(subkey == "position") { //"10%", "10px", "10% 10%", "10px 10px"
      style[key + "/" + subkey] = value;
    }
    else if(subkey == "size") {//"10%", "10% 10%", "cover", "contain"
      style[key + "/" + subkey] = value;
    }
    else if(subkey == "repeat") {
      std::vector<std::string> _flex_possible_values = {"repeat-x","repeat-y","repeat","none"};
      if(_flex_possible_values.end() == std::find(_flex_possible_values.begin(), _flex_possible_values.end(), value)) {
        auto joined_view = _flex_possible_values | std::views::join_with('|');//std::ranges::to<std::string>(joined_view);
        LL(WARNING, LVSIM) << key << "/" << subkey << "'" << value << "' is not valid. Acceptable values are: " << std::ranges::to<std::string>(joined_view);
        return;
      }
      style[key + "/" + subkey] = value;
    }
    else {
      LL(WARNING, LVSIM) << key << "/" << subkey << " is not a valid attribute";        
      return;
    }
    attributes.push_back(subkey);
  });
  return {style, attributes};
}

std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> 
ImageBuilder::fillBackgroundColor(int color, int obj_width, int obj_height) {

  std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> img_dsc = LeleImageConverter::generateImgDsc(obj_width, obj_height, 3);
  if(!img_dsc) {
    LL(FATAL, LVSIM) << "Failed to generate image for background color";
  }
  LeleImageConverter::fillImgDsc(img_dsc->get(), color);
  return img_dsc;
}


ImageBuilder::Res
ImageBuilder::drawBackgroundImage(
  const std::string &prefix,
  const std::string &src, 
  const std::vector<std::string> &style_keys,
  const std::map<std::string, std::optional<LeleStyle::StyleValue>> &style_map,
  int obj_width, 
  int obj_height) {

    Res res;
    res._img_dsc = LeleImageConverter::generateImgDsc(src);
    if(!res._img_dsc) {
        LOG(FATAL, LVSIM, "Failed in generating image description");
        return res;
    }
    for(const auto &key: style_keys) {
      const auto &value = style_map.at(key);
      if(key == (prefix + "/size")) {
        std::string val = std::get<std::string>(value.value());
        if(val == "cover") {
          res._img_dsc = resizeContentToFillContainerPotentiallyCroppingContent(res._img_dsc.value().get(), obj_width, obj_height);
        }
        else if(val == "contain") {
          res._img_dsc = resizeToShowEntireContentPotentiallyLeavingEmptySpace(res._img_dsc.value().get(), obj_width, obj_height);
        }
        else if(!val.empty()) {
          res._img_dsc = resizeImageWithValuesParsedFromJson(res._img_dsc.value().get(), val, obj_width, obj_height);
        }
        if(!res._img_dsc) {
          LL(FATAL, LVSIM) << "Failed in processing " << prefix << "/size";
          return res;
        }
      }
      else if(key == (prefix + "/position")) {
        std::tie(res._offset._x, res._offset._y) = parseImageJsonPosition(value, obj_width, obj_height);
      }
      else if(key == (prefix + "/rotation/pivot/x")) {
        // background_rotation_pivot._x = std::stoi(std::get<std::string>(value.value()));
        res._rotation_pivot._x = std::get<int>(value.value());
      }      
      else if(key == (prefix + "/rotation/pivot/y")) {
        // background_rotation_pivot._y = std::stoi(std::get<std::string>(value.value()));
        res._rotation_pivot._y = std::get<int>(value.value());
      }
      else if(key == (prefix + "/rotation/angle")) {
        res._rotation_angle = std::get<float>(value.value());
        res._img_dsc = LeleImageConverter::rotateImg(res._img_dsc.value().get(), res._rotation_pivot._x, res._rotation_pivot._y, res._rotation_angle);
        if(!res._img_dsc) {
          LL(FATAL, LVSIM) << "Failed in processing " << prefix << "/rotate";
          return res;
        }
        // std::string filename("rotated.");
        // filename += std::to_string(background_rotation_angle);
        // filename += ".png";
        // LeleImageConverter::saveGdImage(filename.c_str(), img_dsc.value().get());
      }
      else if(key == (prefix + "/repeat")) {
        std::string val = std::get<std::string>(value.value());
        if(val == "repeat-x"){
          res._img_dsc = LeleImageConverter::tileImg(res._img_dsc.value().get(), obj_width, obj_height, LeleImageConverter::TileRepeat::RepeatX, res._offset._x, res._offset._y);
        }
        else if(val == "repeat-y"){
          res._img_dsc = LeleImageConverter::tileImg(res._img_dsc.value().get(), obj_width, obj_height, LeleImageConverter::TileRepeat::RepeatY, res._offset._x, res._offset._y);
        }
        else if(val == "repeat"){
          res._img_dsc = LeleImageConverter::tileImg(res._img_dsc.value().get(), obj_width, obj_height, LeleImageConverter::TileRepeat::RepeatXY, res._offset._x, res._offset._y);
        }
        if(!res._img_dsc) {
          LL(FATAL, LVSIM) << "Failed in " << prefix << "/repeat";
          return res;
        }
      }
    }
    if(res._img_dsc.value().get()->header.w > obj_width || 
        res._img_dsc.value().get()->header.h > obj_height) {
        res._img_dsc = LeleImageConverter::cropImg(res._img_dsc.value().get(), 0, 0, obj_width, obj_height);
    }
    if(!res._img_dsc) {
      LL(FATAL, LVSIM) << "Failed in cropping image";
      return res;
    }
    return res;
}
