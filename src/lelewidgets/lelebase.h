#pragma once

#include "lelewidgetfactory.h"

#include <json_utils/json_utils.h>
#include <lvgl/lvgl_private.h>
#include <typeinfo>

#include "lelestyle.h"

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

  const std::string &getClassName() const {
    return _class_name;
  }
  const std::string &getId() const {
    return _id;
  }
  lv_obj_t *getLvObj() const {
    return _lv_obj;
  }
  void setLvObj(lv_obj_t *obj) {
    _lv_obj = obj;
  }
  void setParent(LeleBase *parent) {
    _lele_parent = parent;
    _lele_style->setLeleParent(parent);
  }
  const LeleStyle *style() const {
    return _lele_style;
  }
  virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr);
  virtual void setStyle();
  static void EventCallback(lv_event_t * e);
  virtual void eventCallback(lv_event_t * e);
  protected:
  std::string _class_name = "N/A";
  std::string _id;
  lv_obj_t *_lv_obj = nullptr;
  LeleBase *_lele_parent = nullptr;
  lv_style_t _style = {0};
  std::vector<std::pair<std::string, LeleWidgetFactory::Token>> _tokens;
  LeleStyle _null_pos;
  LeleStyle *_lele_style = &_null_pos;
};
