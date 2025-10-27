#pragma once

#include "lelewidgetfactory.h"

#include <json_utils/json_utils.h>
#include <lv_image_converter/lv_image_converter.h>
#include <lvgl/lvgl_private.h>
#include <optional>
#include <typeinfo>

//need to include all the types in   using Token = std::variant<
#include "leleevent.h"
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
  friend std::ostream& operator<<(std::ostream& os, const LeleBase& p);

  const std::string &className() const {
    return _class_name;
  }
  const std::string &id() const {
    return _id;
  }
  std::vector<LeleBase *> getLeleObj(const std::string &obj_name) const;
  bool enabled() const { return _enabled; }
  lv_obj_t *getLvObj() const {
    return _lv_obj;
  }
  void setLvObj(lv_obj_t *obj) {
    _lv_obj = obj;
  }
  void setParent(LeleBase *parent) {
    _lele_parent = parent;
    _lele_styles.setLeleParent(parent);
  }
  const LeleStyles *styles() const {
    return &_lele_styles;
  }
  virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr);
  virtual void setStyle();
  virtual void setObjAlignStyle(lv_obj_t *lv_obj);
  virtual void setTextAlignStyle(lv_obj_t *lv_obj);
  virtual void show();
  virtual void hide();
  static void EventCallback(lv_event_t *e);
  virtual bool eventCallback(LeleEvent &&e);
  protected:
  void drawBackgroundImage(std::optional<LeleStyle::StyleValue> value, int obj_width, int obj_height);
  std::tuple<int,int> parseBackgroundPosition(
    const std::optional<LeleStyle::StyleValue> &value, int container_width, int container_height) const;

  std::string _class_name = "N/A";
  std::string _id;
  bool _enabled = true;
  lv_obj_t *_lv_obj = nullptr;
  LeleBase *_lele_parent = nullptr;
  lv_style_t _style = {0};
  std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> _bg_img;
  std::vector<std::pair<std::string, LeleWidgetFactory::Token>> _tokens;
  LeleStyles _lele_styles;
};
