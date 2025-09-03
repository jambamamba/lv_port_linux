#include "lelewidgetfactory.h"

#include "lelenullwidget.h"
#include "leletabview.h"
#include "lelelabel.h"
#include "leletextbox.h"

LOG_CATEGORY(LVSIM, "LVSIM");

namespace {
static std::vector<std::pair<std::string, std::string>> tokenize(const std::string &json_str) {
    std::vector<std::pair<std::string, std::string>> res;
    if(json_str.empty()) {
        return res;
    }
    const cJSON *json = cJSON_Parse(json_str.c_str());
    cJSON *item = nullptr;
    int idx = 0;
    cJSON_ArrayForEach(item, json) {
        std::string item_num = std::string("@") + std::to_string(idx);
        // LOG(DEBUG, LVSIM, "Process item: %s:%s\n", item_num.c_str(), item->string);
        std::string key = !item->string ? 
            item_num : 
            item->string;
        if(cJSON_IsString(item)) {
            res.emplace_back(std::pair<std::string, std::string>
                {key, cJSON_GetStringValue(item)}
            );
        }
        else if(cJSON_IsNumber(item) || cJSON_IsTrue(item) || cJSON_IsFalse(item) ||
            cJSON_IsBool(item) || cJSON_IsNull(item)){
            res.emplace_back(std::pair<std::string, std::string>
                {key, std::to_string(cJSON_GetNumberValue(item))}
            );
        }
        else if(cJSON_IsObject(item) || cJSON_IsArray(item)) {
            if(item->string) {
                res.emplace_back(std::pair<std::string, std::string>
                    {key, cJSON_Print(item)}
                );
            }
            else {
                std::vector<std::pair<std::string, std::string>> subtokens = tokenize(cJSON_Print(item));
                res.insert( res.end(), subtokens.begin(), subtokens.end() );
                // for(const auto &pair: subtokens) {
                //     LOG(DEBUG, LVSIM, "@@@ %s:%s\n", pair.first.c_str(), pair.second.c_str());
                // }
            }
        }
        else {
            LOG(WARNING, LVSIM, "Unknown and unhandled item: %s\n", key);
        }
        ++idx;
    }
    // for(const auto &pair: res) {
    //     LOG(DEBUG, LVSIM, "Processed token %s:%s\n", pair.first.c_str(), pair.second.c_str());
    // }
    return res;
}
}//namespace

namespace LeleWidgetFactory {

std::vector<std::pair<std::string, Token>> fromJson(
    const std::string &json_str, lv_obj_t *parent) {

    std::vector<std::pair<std::string, Token>> res;
    // const cJSON *json = readJson(json_str.c_str());
    auto tokens = tokenize(json_str);
    for(const auto &[lhs, rhs]: tokens) {
        Token token;
        if(lhs == "tabview") {
            token = std::make_unique<LeleTabView>(rhs, parent);
        }
        else if(lhs == "tabs") {
            token = std::make_unique<LeleTabView::Tabs>(rhs, parent);
        }
        else if(lhs == "tab") {
            token = std::make_unique<LeleTabView::Tab>(rhs, parent);
        }
        else if(lhs == "tab_button") {
            token = std::make_unique<LeleTabView::TabButton>(rhs, parent);
        }
        else if(lhs == "tab_content") {
            token = std::make_unique<LeleTabView::TabContent>(rhs, parent);
        }
        else if(lhs == "pos") {
            token = std::make_unique<LelePos>(rhs, parent);
        }
        else {
            token = rhs;
        }
        res.emplace_back(
            std::make_pair<std::string, Token>(
                std::string(lhs), std::move(token)));
    }
    return res;
}

std::vector<std::pair<std::string, Token>> fromConfig(const std::string &config_json) {
    const cJSON* root = readJson(config_json.c_str());
    if(!root) {
        LOG(FATAL, LVSIM, "Failed to failed to load file: '%s'\n", config_json.c_str());
        return std::vector<std::pair<std::string, LeleWidgetFactory::Token>>();
    }
    return LeleWidgetFactory::fromJson(cJSON_Print(root));
}
// FactoryInput::FactoryInput(
//     const std::string &json_str,
//     int container_width,
//     int container_height
// ){
//     const cJSON *json = readJson(json_str.c_str());
//     cJSON *item = nullptr;
//     cJSON_ArrayForEach(item, json) {
//       // LOG(DEBUG, LVSIM, "tab content: %s\n", item->string);
//       if(strcmp(item->string, "text") == 0) {
//           _text = cJSON_GetStringValue(item);
//       }
//       else if(strcmp(item->string, "pos") == 0) {
//           _pos = LelePos::fromJson(container_width, container_height, item);
//       }
//       else if(strcmp(item->string, "title") == 0) {
//           _title = cJSON_GetStringValue(item);
//       }
//       else if(strcmp(item->string, "subtitle") == 0) {
//           _subtitle = cJSON_GetStringValue(item);
//       }
//       else if(strcmp(item->string, "name") == 0) {
//           _name = cJSON_GetStringValue(item);
//       }
//       else if(strcmp(item->string, "img") == 0) {
//           _img = cJSON_GetStringValue(item);
//       }
//       else if(strcmp(item->string, "fgcolor") == 0) {
//           _fgcolor = cJSON_GetStringValue(item);
//       }
//       else if(strcmp(item->string, "bgcolor") == 0) {
//           _bgcolor = cJSON_GetStringValue(item);
//       }
//       else if(strcmp(item->string, "active_tab_bgcolor") == 0) {
//           _active_tab_bgcolor = cJSON_GetStringValue(item);
//       }
//       else if(strcmp(item->string, "active_tab_bottom_border_color") == 0) {
//           _active_tab_bottom_border_color = cJSON_GetStringValue(item);
//       }
//       else if(strcmp(item->string, "tabview") == 0) {
//         _widgets.emplace_back(
//             std::pair<std::string, std::string>{item->string, cJSON_Print(item)});
//       }
//       else if(strcmp(item->string, "tabs") == 0) {
//         if(cJSON_IsArray(item)) {
//             cJSON *subitem = nullptr;
//             cJSON_ArrayForEach(subitem, item) {
//                 if(strcmp(subitem->string, "tab") == 0) {
//                     _widgets.emplace_back(
//                         std::pair<std::string, std::string>{item->string, cJSON_Print(item)});
//                 }
//             }
//         }
//       }
//       else if(strcmp(item->string, "tab") == 0) {
//         _widgets.emplace_back(
//             std::pair<std::string, std::string>{item->string, cJSON_Print(item)});
//       }
//       else if(strcmp(item->string, "tab_button") == 0) {
//         _widgets.emplace_back(
//             std::pair<std::string, std::string>{item->string, cJSON_Print(item)});
//       }
//       else if(strcmp(item->string, "tab_content") == 0) {
//         _widgets.emplace_back(
//             std::pair<std::string, std::string>{item->string, cJSON_Print(item)});
//       }
//       else {
//         LOG(WARNING, LVSIM, "Ignoring json item: %s\n", item->string);
//       }
//     }
// }

// std::vector<std::unique_ptr<LeleBase>> createLeleWidget(
//     const std::string &json_str,
//     lv_obj_t *parent,
//     int container_width, 
//     int container_height) {

//     std::vector<std::unique_ptr<LeleBase>> res;
//     LeleWidgetFactory::FactoryInput fi(
//         json_str, container_width, container_height);
//     for(const auto &[widget_type, json_str]: fi._widgets) {
//         if(widget_type != "tabview") {
//             LeleWidgetFactory::FactoryInput fi(json_str);
//             res.emplace_back(std::make_unique<LeleTabView>(
//                 fi._title,
//                 fi._subtitle,
//                 fi._img,
//                 fi._fgcolor, 
//                 fi._bgcolor, 
//                 fi._active_tab_bgcolor, 
//                 fi._active_tab_bottom_border_color, 
//                 fi._widgets
//             ));
//         }
//         else if(widget_type == "tab") {
//             return std::make_unique<LeleTabView::Tab>(
//                 fi._widgets);
//         }
//         else if(widget_type == "tab_button") {
//             // return std::make_unique<LeleTabView::Tab>(
//             //     fi._widgets);
//         }    
//         else if(widget_type == "tab_content") {
//             // return std::make_unique<LeleTabView::Tab>(
//             //     fi._widgets);
//         }    
//         else if(widget_type == "LeleLabel") {
//             return std::make_unique<LeleLabel>(
//                 parent, 
//                 fi._text, 
//                 fi._pos.x(), 
//                 fi._pos.y(), 
//                 fi._pos.width(), 
//                 fi._pos.height()
//             );
//         }
//         else if(widget_type == "LeleTextbox") {
//             return std::make_unique<LeleTextbox>(
//                 parent, 
//                 fi._text, 
//                 fi._pos.x(), 
//                 fi._pos.y(), 
//                 fi._pos.width(), 
//                 fi._pos.height()
//             );
//         }    
//     }

//     return std::make_unique<LeleNullWidget>();
// }
}//LeleWidgetFactory