#pragma once

#include "lelebase.h"

#include <map>
#include <optional>
#include <lv_image_converter/lv_image_converter.h>

class LeleImage : public LeleBase  {
  public:
  LeleImage(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  std::string getSrc() const;


  struct Rotation {
    float _angle = 0.;
    int _pivot_x = 0;
    int _pivot_y = 0;
  };
  struct Offset {
    int _offset_x = 0;
    int _offset_y = 0;
  };
  struct Scale {
    int _percent_x = 0;
    int _percent_y = 0;
  };
  protected:
  std::optional<LeleImage::Rotation> parseRotation(const std::string &json_str);
  
  std::string _src;
  std::string _name;
  std::optional<lv_blend_mode_t> _blendmode;
  std::optional<lv_image_align_t> _align;
  std::optional<bool> _antialias;
  std::optional<Rotation> _rotation;
  std::optional<Offset> _offset;
  std::optional<Scale> _scale;
  std::map<std::string, std::optional<AutoFreeSharedPtr<lv_image_dsc_t>>> _images;
};
