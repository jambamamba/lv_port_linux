#pragma once

#include "leleobject.h"

#include <map>
#include <optional>
#include <lv_image_converter/lv_image_converter.h>

class LeleImage : public LeleObject  {
  public:
  LeleImage(const std::string &json_str);
  virtual bool fromJson(const std::string &json_str) override;
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  std::string getSrc() const;


  struct XY {
    int _x = 0;
    int _y = 0;
  };
  protected:
  
  std::string _src;
  std::string _name;
  std::optional<lv_blend_mode_t> _blendmode;
  std::optional<lv_image_align_t> _align;
  std::optional<bool> _antialias;
  std::optional<float> _rotation_angle;
  std::optional<XY> _rotation_pivot;
  std::optional<XY> _offset;
  std::optional<XY> _scale;
  std::map<std::string, std::optional<AutoFreeSharedPtr<lv_image_dsc_t>>> _images;
};
