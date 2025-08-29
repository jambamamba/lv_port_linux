#pragma once

#include "lelepos.h"
#include "lelewidgetfactory.h"

#include <typeinfo>

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN && LV_MEM_SIZE < (38ul * 1024ul)
    #error Insufficient memory for lv_demo_widgets. Please set LV_MEM_SIZE to at least 38KB (38ul * 1024ul).  48KB is recommended.
#endif

typedef enum {
    DISP_SMALL,
    DISP_MEDIUM,
    DISP_LARGE,
} disp_size_t;

class LeleBase {
  public:
  LeleBase() = default;
  virtual ~LeleBase() = default;

  lv_obj_t *getLvObj() const {
    return _lv_obj;
  }
  void setLvObj(lv_obj_t *obj) {
    _lv_obj = obj;
  }
  void addChild(std::unique_ptr<LeleBase> &&child) {
    _children.emplace_back(std::move(child));
  }

  std::vector<std::unique_ptr<LeleBase>> fromJson(
    const std::string &json_str,
    lv_obj_t *parent = lv_screen_active(), 
    int container_width = lv_obj_get_width(lv_screen_active()), 
    int container_height = lv_obj_get_height(lv_screen_active())) const {

    return LeleWidgetFactory::createLeleWidget(json_str, parent, container_width, container_height);
  }
  protected:
  lv_obj_t *_lv_obj = nullptr;
  std::vector<std::unique_ptr<LeleBase>> _children;
};
