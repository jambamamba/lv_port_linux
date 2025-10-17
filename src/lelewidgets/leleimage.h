#pragma once

#include "lelebase.h"

#include <map>
#include <optional>
#include <lv_image_converter/mainlib.h>

class LeleImage : public LeleBase  {
  public:
  LeleImage(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  void setSrc(const std::string &src);
  std::string getSrc() const;


  struct Rotation {
    float _angle = 0.;
    int _pivot_x = 0;
    int _pivot_y = 0;
  };
  struct Scale {
    int _percent_x = 0;
    int _percent_y = 0;
  };
  protected:
  void parseRotation(const std::string &json_str);
  
  std::string _src;
  std::string _name;
  int _x;
  int _y;
  Rotation _rotation;
  Scale _scale;
  lv_blend_mode_t _blendmode;
  lv_image_align_t _align;
  bool _antialias;
  std::map<std::string, std::optional<AutoFreeSharedPtr<lv_image_dsc_t>>> _images;
};
