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
  protected:
  std::string _src;
  std::map<std::string, std::optional<AutoFreeSharedPtr<lv_image_dsc_t>>> _images;
};
