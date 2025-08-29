#include "lelewidgetfactory.h"

#include "lelenullwidget.h"
#include "leletabview.h"
#include "lelelabel.h"
#include "leletextbox.h"

LOG_CATEGORY(LVSIM, "LVSIM");

namespace LeleWidgetFactory {

// std::string FactoryInput::lastValue(const std::string &key) const {
//     if(key == "text") {
//         return _text.empty() ? "" : _text.back();
//     }
//     else if(key == "title") {
//         return _title.empty() ? "" : _title.back();
//     }
//     else if(key == "subtitle") {
//         return _subtitle.empty() ? "" : _subtitle.back();
//     }
//     else if(key == "name") {
//         return _name.empty() ? "" : _name.back();
//     }
//     else if(key == "img") {
//         return _img.empty() ? "" : _img.back();
//     }
//     else if(key == "fgcolor") {
//         return _fgcolor.empty() ? "" : _fgcolor.back();
//     }
//     else if(key == "bgcolor") {
//         return _bgcolor.empty() ? "" : _bgcolor.back();
//     }
//     else if(key == "active_tab_bgcolor") {
//         return _active_tab_bgcolor.empty() ? "" : _active_tab_bgcolor.back();
//     }
//     else if(key == "active_tab_bottom_border_color") {
//         return _active_tab_bottom_border_color.empty() ? "" : _active_tab_bottom_border_color.back();
//     }
//     else if(key == "json_str") {
//         return _json_str.empty() ? "" : _json_str.back();
//     }
//     else if(key == "widget_type") {
//         return _widget_type.empty() ? "" : _widget_type.back();
//     }
//     return "";
// }

// LelePos FactoryInput::lastPos() const {
//     return _pos.empty() ? LelePos() : _pos.back();
// }
FactoryInput::FactoryInput(
    const std::string &json_str,
    int container_width,
    int container_height
){
    const cJSON *json = readJson(json_str.c_str());
    cJSON *item = nullptr;
    cJSON_ArrayForEach(item, json) {
      // LOG(DEBUG, LVSIM, "tab content: %s\n", item->string);
      if(strcmp(item->string, "text") == 0) {
          _text = cJSON_GetStringValue(item);
      }
      else if(strcmp(item->string, "pos") == 0) {
          _pos = LelePos::fromJson(container_width, container_height, item);
      }
      else if(strcmp(item->string, "title") == 0) {
          _title = cJSON_GetStringValue(item);
      }
      else if(strcmp(item->string, "subtitle") == 0) {
          _subtitle = cJSON_GetStringValue(item);
      }
      else if(strcmp(item->string, "name") == 0) {
          _name = cJSON_GetStringValue(item);
      }
      else if(strcmp(item->string, "img") == 0) {
          _img = cJSON_GetStringValue(item);
      }
      else if(strcmp(item->string, "fgcolor") == 0) {
          _fgcolor = cJSON_GetStringValue(item);
      }
      else if(strcmp(item->string, "bgcolor") == 0) {
          _bgcolor = cJSON_GetStringValue(item);
      }
      else if(strcmp(item->string, "active_tab_bgcolor") == 0) {
          _active_tab_bgcolor = cJSON_GetStringValue(item);
      }
      else if(strcmp(item->string, "active_tab_bottom_border_color") == 0) {
          _active_tab_bottom_border_color = cJSON_GetStringValue(item);
      }
      else if(strcmp(item->string, "tabview") == 0) {
        _widget.emplace_back({"LeleTabView", cJSON_Print(item)});
      }
      else if(strcmp(item->string, "tabs") == 0) {
          const cJSON *json_tabs = objFromJson(item, "tabs");
          if(cJSON_IsArray(json_tabs)) {
            _widget.emplace_back({"LeleTabView::Tab", cJSON_Print(item)});
          }
      }
      else if(strcmp(item->string, "tab") == 0) {
        _widget.emplace_back({"LeleTabView::Tab", cJSON_Print(item)});
      }
      else if(strcmp(item->string, "tab_button") == 0) {
        _widget.emplace_back({"LeleTabView::TabButton", cJSON_Print(item)});
      }
      else if(strcmp(item->string, "tab_content") == 0) {
        _widget.emplace_back({"LeleTabView::TabContent", cJSON_Print(item)});
      }
      else {
        LOG(WARNING, LVSIM, "Ignoring json item: %s\n", item->string);
      }
    }
}

std::vector<std::unique_ptr<LeleBase>> createLeleWidget(
    const std::string &json_str,
    lv_obj_t *parent,
    int container_width, 
    int container_height) {

    std::vector<std::unique_ptr<LeleBase>> res;
    LeleWidgetFactory::FactoryInput fi(json_str, container_width, container_height)
    for(const auto &[widget_type, json_str]: fi._widget) {
        if(widget_type == "LeleTabView") {
            LeleWidgetFactory::FactoryInput fi(widget._json_str);
            res.emplace_back(std::make_unique<LeleTabView>(
                fi._title,
                fi._subtitle, 
                fi._img,
                fi._fgcolor, 
                fi._bgcolor, 
                fi._active_tab_bgcolor, 
                fi._active_tab_bottom_border_color, 
                fi._widget
            ));
        }
        else if(widget_type == "LeleTabView::Tab") {
            LeleWidgetFactory::FactoryInput fi(widget._json_str);
            res.emplace_back(std::make_unique<LeleTabView::Tab>(
                fi._title,
                fi._img,
                fi._widget["LeleTabView::TabButton"],
                fi._widget["LeleTabView::TabContent"]
            ));
        }
    }
    else if(fi.lastValue("_widget_type") == "LeleTabView::Tab") {
        LeleWidgetFactory::FactoryInput fi2(fi.lastValue("_json_str"));
        for(const auto &tab_json : fi._children_json) {
            LeleWidgetFactory::FactoryInput fi2(tab_json);
            return std::make_unique<LeleTabView::Tab>(fi2._json_str);
        }
    }
    else if(fi._widget_type == "LeleTabView::Tab") {
        return std::make_unique<LeleTabView::Tab>(
            fi._name, 
            fi._img, 
            fi._json);
    }
    else if(fi._widget_type == "LeleLabel") {
        return std::make_unique<LeleLabel>(
            parent, 
            fi._text, 
            fi._pos.x(), 
            fi._pos.y(), 
            fi._pos.width(), 
            fi._pos.height()
        );
    }
    else if(fi._widget_type == "LeleTextbox") {
        return std::make_unique<LeleTextbox>(
            parent, 
            fi._text, 
            fi._pos.x(), 
            fi._pos.y(), 
            fi._pos.width(), 
            fi._pos.height()
        );
    }
    return std::make_unique<LeleNullWidget>();
}
}//LeleWidgetFactory