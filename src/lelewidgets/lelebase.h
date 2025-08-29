#pragma once

#include "lelepos.h"

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
  template <typename LeleWidget>
  std::unique_ptr<LeleWidget> fromJson(lv_obj_t *parent, int container_width, int container_height, const cJSON *json) {
    
    std::string widget_type = typeid(LeleWidget).name();
    LelePos pos;
    cJSON *item = nullptr;
    std::string text, title, subtitle, name, img, fgcolor, bgcolor, 
      active_tab_bgcolor, active_tab_bottom_border_color, content;
    std::vector<std::unique_ptr<LeleTabView::Tab>> tabs;
    cJSON_ArrayForEach(item, json) {
        // LOG(DEBUG, LVSIM, "tab content: %s\n", item->string);
        if(strcmp(item->string, "text") == 0) {
          text= cJSON_GetStringValue(item);
        }
        else if(strcmp(item->string, "pos") == 0) {
          pos = LelePos::fromJson(container_width, container_height, item);
        }
        else if(strcmp(item->string, "title") == 0) {
          title = cJSON_GetStringValue(item);
        }
        else if(strcmp(item->string, "subtitle") == 0) {
          subtitle = cJSON_GetStringValue(item);
        }
        else if(strcmp(item->string, "name") == 0) {
          name = cJSON_GetStringValue(item);
        }
        else if(strcmp(item->string, "img") == 0) {
          img = cJSON_GetStringValue(item);
        }
        else if(strcmp(item->string, "fgcolor") == 0) {
          fgcolor = cJSON_GetStringValue(item);
        }
        else if(strcmp(item->string, "bgcolor") == 0) {
          bgcolor = cJSON_GetStringValue(item);
        }
        else if(strcmp(item->string, "active_tab_bgcolor") == 0) {
          active_tab_bgcolor = cJSON_GetStringValue(item);
        }
        else if(strcmp(item->string, "active_tab_bottom_border_color") == 0) {
          active_tab_bottom_border_color = cJSON_GetStringValue(item);
        }
        else if(strcmp(item->string, "content") == 0) {
          content = cJSON_Print(item);//This has bug: cJSON_Duplicate(item, true);//dont forget to cJSON_Delete
        }
        else if(strcmp(item->string, "tabs") == 0) {
          const cJSON *json_tabs = objFromJson(item, "tabs");
          if(cJSON_IsArray(json_tabs)) {
              cJSON *json_tab = nullptr;
              cJSON_ArrayForEach(json_tab, json_tabs) {
                  tabs.emplace_back(
                    LeleTabView::Tab::fromJson(json_tab));//osm todo
              }
          }
        }
      }
      if(widget_type == "LeleTabView") {
        return std::make_unique<LeleWidget>(
          title, 
          subtitle, 
          img, 
          fgcolor, 
          bgcolor, 
          active_tab_bgcolor, 
          active_tab_bottom_border_color, 
          std::move(tabs));
      }
      return std::make_unique<LeleWidget>(
        parent, 
        text, 
        pos.x(), 
        pos.y(), 
        pos.width(), 
        pos.height()
      );
  }
  protected:
  lv_obj_t *_lv_obj = nullptr;
  std::vector<std::unique_ptr<LeleBase>> _children;
};
