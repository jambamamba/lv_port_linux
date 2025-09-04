#pragma once

#include "lelewidgetfactory.h"

#include <json_utils/json_utils.h>
#include <lvgl/lvgl_private.h>
#include <typeinfo>

#include "lelepos.h"

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
  LeleBase(const std::string &json_str = "");
  virtual ~LeleBase();

  lv_obj_t *getLvObj() const {
    return _lv_obj;
  }
  void setLvObj(lv_obj_t *obj) {
    _lv_obj = obj;
  }
  const LelePos *pos() const {
    return _pos;
  }
  virtual lv_obj_t *createLvObj(lv_obj_t *lv_parent = lv_screen_active(), LeleBase *lele_parent = nullptr);
  protected:
  lv_obj_t *_lv_obj = nullptr;
  LeleBase *_lele_parent = nullptr;
  lv_style_t _style = {0};
  std::vector<std::pair<std::string, LeleWidgetFactory::Token>> _tokens;
  LelePos _null_pos;
  LelePos *_pos = &_null_pos;
};
