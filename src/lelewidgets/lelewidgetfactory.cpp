#include "lelewidgetfactory.h"

#include "lelebutton.h"
#include "leleevent.h"
#include "lelelabel.h"
#include "lelenullwidget.h"
#include "lelestackview.h"
#include "leletabview.h"
#include "leletextbox.h"
#include "leleview.h"

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
    const std::string &json_str) {

    std::vector<std::pair<std::string, Token>> res;
    // const cJSON *json = readJson(json_str.c_str());
    auto tokens = tokenize(json_str);
    for(const auto &[lhs, rhs]: tokens) {
        Token token;
        if(lhs == "tabview") {
            token = std::make_unique<LeleTabView>(rhs);
        }
        else if(lhs == "tabs") {
            token = std::make_unique<LeleTabView::Tabs>(rhs);
        }
        else if(lhs == "tab") {
            token = std::make_unique<LeleTabView::Tab>(rhs);
        }
        else if(lhs == "tab_header") {
            token = std::make_unique<LeleTabView::TabHeader>(rhs);
        }
        else if(lhs == "tab_content") {
            token = std::make_unique<LeleTabView::TabContent>(rhs);
        }
        //
        else if(lhs == "stackview") {
            token = std::make_unique<LeleStackView>(rhs);
        }
        else if(lhs == "views") {
            token = std::make_unique<LeleViews>(rhs);
        }
        else if(lhs == "view") {
            token = std::make_unique<LeleView>(rhs);
        }
        else if(lhs == "view_header") {
            token = std::make_unique<LeleViewHeader>(rhs);
        }
        //
        else if(lhs == "label") {
            token = std::make_unique<LeleLabel>(rhs);
        }
        else if(lhs == "textbox") {
            token = std::make_unique<LeleTextbox>(rhs);
        }
        else if(lhs == "view") {
            token = std::make_unique<LeleView>(rhs);
        }
        else if(lhs == "buttons") {
            token = std::make_unique<LeleButtons>(rhs);
        }
        else if(lhs == "button") {
            token = std::make_unique<LeleButtons::LeleButton>(rhs);
        }
        else if(lhs == "style") {
            token = std::make_unique<LeleStyle>(rhs);
        }
        else if(lhs == "event") {
            token = std::make_unique<LeleEvent>(rhs);
        }
        else {
            token = rhs;
        }
        // LOG(DEBUG, LVSIM, "lhs:%s\n", lhs.c_str());
        res.emplace_back(
            std::make_pair<std::string, Token>(
                std::string(lhs), std::move(token)));
    }
    return res;
}

std::vector<std::pair<std::string, Token>> fromConfig(const std::string &config_json) {
    static LeleBase _root_widget;
    _root_widget.setLvObj(lv_screen_active());
    const cJSON* root = readJson(config_json.c_str());
    if(!root) {
        LOG(FATAL, LVSIM, "Failed to failed to load file: '%s'\n", config_json.c_str());
        return std::vector<std::pair<std::string, LeleWidgetFactory::Token>>();
    }
    auto tokens = LeleWidgetFactory::fromJson(cJSON_Print(root));
    for (const auto &[key, token]: tokens) {
        LOG(DEBUG, LVSIM, "Process token with key: %s\n", key.c_str());
        if (std::holds_alternative<std::unique_ptr<LeleBase>>(token)) {
            auto &value = std::get<std::unique_ptr<LeleBase>>(token);
            value->createLvObj(&_root_widget);
        }
    }
    return tokens;
}
}//LeleWidgetFactory