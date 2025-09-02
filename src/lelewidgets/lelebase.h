#pragma once

#include "lelewidgetfactory.h"

#include <json_utils/json_utils.h>
#include <lvgl/lvgl_private.h>
#include <typeinfo>

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN && LV_MEM_SIZE < (38ul * 1024ul)
    #error Insufficient memory for lv_demo_widgets. Please set LV_MEM_SIZE to at least 38KB (38ul * 1024ul).  48KB is recommended.
#endif

typedef enum {
    DISP_SMALL,
    DISP_MEDIUM,
    DISP_LARGE,
} disp_size_t;

class LelePos;
class LeleBase {
  public:
  LeleBase(const std::string &json_str = "", lv_obj_t *parent = lv_screen_active());
  virtual ~LeleBase();

  lv_obj_t *getLvObj() const {
    return _lv_obj;
  }
  void setLvObj(lv_obj_t *obj) {
    _lv_obj = obj;
  }
  virtual lv_obj_t *createLvObj(lv_obj_t *parent = lv_screen_active(), int x = 0, int y = 0, int width = 0, int height = 0, const std::string &corner_radius = "") const;
  void addChild(std::unique_ptr<LeleBase> &&child) {
    _children.emplace_back(std::move(child));
  }
  protected:
  lv_obj_t *_lv_obj = nullptr;
  // lv_obj_t *_lv_parent_obj = nullptr;
  lv_style_t _style = {0};
  std::vector<std::unique_ptr<LeleBase>> _children;
  std::vector<std::pair<std::string, LeleWidgetFactory::Token>> _tokens;
  LelePos *_pos = nullptr;
};
