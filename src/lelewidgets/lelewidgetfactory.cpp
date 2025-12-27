#include "lelewidgetfactory.h"

#include <filesystem>
#include <unistd.h>

#include "lelebutton.h"
#include "leleevent.h"
#include "leleimage.h"
#include "lelelabel.h"
#include "lelemessagebox.h"
#include "lelenullwidget.h"
#include "lelestackview.h"
#include "leletabview.h"
#include "leletextbox.h"
#include "leleview.h"

LOG_CATEGORY(LVSIM, "LVSIM");

namespace {
struct cJSONRAII {
    cJSONRAII(const std::string &json_str)
    : _json(cJSON_Parse(json_str.c_str()))
    , _json_str(json_str) {
    }
    ~cJSONRAII(){
        if(_json) {
            cJSON_Delete(_json);
        }
    }
    cJSON *operator()(){
        return _json;
    }
    const std::string &str() const {
        return _json_str;
    }
    protected:
    cJSON *_json;
    const std::string &_json_str;
};

static std::vector<std::pair<std::string, std::string>> tokenize(const std::string &json_str) {
    std::vector<std::pair<std::string, std::string>> res;
    if(json_str.empty()) {
        return res;
    }
    cJSONRAII json(json_str);
    cJSON *item = nullptr;
    int idx = 0;
    cJSON_ArrayForEach(item, json()) {
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
            LOG(WARNING, LVSIM, "Unknown and unhandled item: %s\n", key.c_str());
        }
        ++idx;
    }
    // for(const auto &pair: res) {
    //     LOG(DEBUG, LVSIM, "Processed token %s:%s\n", pair.first.c_str(), pair.second.c_str());
    // }
    return res;
}


const cJSON* jsonFromConfig(const std::string &config_json) {

    const cJSON* root = nullptr;
    std::error_code ec;
    if(std::filesystem::exists(config_json, ec)) {
        LOG(DEBUG, LVSIM, "Loading JSON from file: '%s'.\n", config_json.c_str());
        root = readJson(config_json.c_str());
    }
    else {
        LOG(DEBUG, LVSIM, "Not a file: '%s', next check if it is a JSON string.\n", config_json.c_str());
        root = cJSON_Parse(config_json.c_str());
    }
    if(!root) {
        LOG(WARNING, LVSIM, "Failed to load JSON: '%s'\n", config_json.c_str());
        return nullptr;
    }
    LOG(DEBUG, LVSIM, "Successfully loaded JSON: '%s'\n", config_json.c_str());
    return root;
}

auto leleObjectsFromJson(LeleObject *parent, const std::string &json_str) {
    auto nodes = LeleWidgetFactory::fromJson(json_str);
    for (const auto &[key, node]: nodes) {
        LOG(DEBUG, LVSIM, "Process node with key: %s\n", key.c_str());
        if (std::holds_alternative<std::unique_ptr<LeleObject>>(node)) {
            auto &value = std::get<std::unique_ptr<LeleObject>>(node);
            value->createLvObj(parent);
        }
        else if (std::holds_alternative<std::unique_ptr<LeleStyle>>(node)) {
            auto &value = std::get<std::unique_ptr<LeleStyle>>(node);
            parent->addStyle(value.get());
        }
        else if (std::holds_alternative<std::unique_ptr<LeleEvent>>(node)) {
        }
    }
    return nodes;
}

auto leleStylesFromJson(const std::string &json_str) {
    std::vector<std::unique_ptr<LeleStyle>> styles;
    auto nodes = LeleWidgetFactory::fromJson(json_str);
    for (auto &[key, token]: nodes) {
        if (std::holds_alternative<std::unique_ptr<LeleStyle>>(token)) {
            if(key == "style") {
                styles.emplace_back(
                    std::move(
                        std::get<std::unique_ptr<LeleStyle>>(token)));
            }
        }
    }
    return styles;
}

}//namespace

namespace LeleWidgetFactory {

std::vector<std::pair<std::string, Node>> fromJson(
    const std::string &json_str) {

    std::vector<std::pair<std::string, Node>> res;
    // const cJSON *json = readJson(json_str.c_str());
    auto tokens = tokenize(json_str);
    for(const auto &[lhs, rhs]: tokens) {
        Node token;
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
        else if(lhs == "messagebox") {
            token = std::make_unique<LeleMessageBox>(rhs);
        }
        else if(lhs == "style") {
            token = std::make_unique<LeleStyle>(rhs);
        }
        else if(lhs == "event") {
            token = std::make_unique<LeleEvent>(rhs);
        }
        else if(lhs == "img" && tokenize(rhs).size() > 0) {
            token = std::make_unique<LeleImage>(rhs);
        }
        else {
            token = rhs;
        }
        // LOG(DEBUG, LVSIM, "lhs:%s\n", lhs.c_str());
        res.emplace_back(
            std::make_pair<std::string, Node>(
                std::string(lhs), std::move(token)));
    }
    return res;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (std::string::npos == first) {
        return str; // String is all whitespace or empty
    }
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, (last - first + 1));
}

void fromJson(const std::string &json_str, std::function<void (const std::string &key, const std::string &value)> callback) {
  if(json_str.empty()) {
    return;
  }
  
  if(json_str.at(0) != '{' || json_str.at(json_str.size() - 1) != '}') {
    return;
  }
  cJSONRAII json(json_str);
  if(cJSON_IsObject(json())) {
    for (const auto &[key, token]: LeleWidgetFactory::fromJson(json_str)) {
        if (std::holds_alternative<std::string>(token)) {
            const std::string &value = std::get<std::string>(token);
            if(callback) {
                callback(key, value);
            }
        }
    }
  }
  else if(cJSON_IsString(json())) {
    callback("", cJSON_GetStringValue(json()));
  }
  else if(cJSON_IsNumber(json())) {
    callback("", std::to_string(cJSON_GetNumberValue(json())));
  }
}

void iterateNodes(
    std::vector<std::pair<std::string, LeleWidgetFactory::Node>> &nodes, 
    int depth,
    std::function<void(LeleObject &lele_object)> on_visit_lele_base_obj
    ) {
    for (const auto &[key,token]: nodes) {
        // LOG(DEBUG, LVSIM, "ITER key:%s\n", key.c_str());
        if (std::holds_alternative<std::unique_ptr<LeleEvent>>(token)) {
            auto &lele_event = std::get<std::unique_ptr<LeleEvent>>(token);
            // LOG(DEBUG, LVSIM, "         lele_event:%s\n", " ");
        }
        else if (std::holds_alternative<std::unique_ptr<LeleStyle>>(token)) {
            auto &lele_style = std::get<std::unique_ptr<LeleStyle>>(token);
            // LOG(DEBUG, LVSIM, "         lele_style:%s\n", " ");
        }
        else if (std::holds_alternative<std::unique_ptr<LeleObject>>(token)) {
            auto &lele_object = std::get<std::unique_ptr<LeleObject>>(token);
            // LOG(DEBUG, LVSIM, "         (%i) lele_object:%s\n", depth, lele_object->id().c_str());
            if(on_visit_lele_base_obj) {
                on_visit_lele_base_obj(*lele_object.get());
            }
            LeleWidgetFactory::iterateNodes(lele_object->children(), depth+1, on_visit_lele_base_obj);
        }
    }
}

namespace {

struct click_counts {
    uint32_t num_clicked;
    uint32_t num_short_clicked;
    uint32_t num_single_clicked;
    uint32_t num_double_clicked;
    uint32_t num_triple_clicked;
    uint32_t num_long_pressed;
    uint8_t short_click_streak;
};

static void click_event_cb(lv_event_t * e)
{
    click_counts * counts = (click_counts *)lv_event_get_user_data(e);

    switch(lv_event_get_code(e)) {
        case LV_EVENT_CLICKED:
            counts->num_clicked++;
            break;
        case LV_EVENT_SHORT_CLICKED:
            counts->num_short_clicked++;
            break;
        case LV_EVENT_SINGLE_CLICKED:
            counts->num_single_clicked++;
            break;
        case LV_EVENT_DOUBLE_CLICKED:
            counts->num_double_clicked++;
            break;
        case LV_EVENT_TRIPLE_CLICKED:
            counts->num_triple_clicked++;
            break;
        case LV_EVENT_LONG_PRESSED:
            counts->num_long_pressed++;
            break;
        default:
            break;
    }

    lv_indev_t * indev = (lv_indev_t *)lv_event_get_param(e);
    counts->short_click_streak = lv_indev_get_short_click_streak(indev);
}

}//namespace
std::vector<std::pair<std::string, Node>> fromConfig(
    LeleObject *parent,
    const std::string &config) {

    if(!parent->getLvObj()) {
        static click_counts counts;
        parent->setLvObj(lv_screen_active());
        lv_obj_add_event_cb(parent->getLvObj(), click_event_cb, LV_EVENT_CLICKED, &counts);
        lv_obj_add_event_cb(parent->getLvObj(), click_event_cb, LV_EVENT_SHORT_CLICKED, &counts);
        lv_obj_add_event_cb(parent->getLvObj(), click_event_cb, LV_EVENT_SINGLE_CLICKED, &counts);
        lv_obj_add_event_cb(parent->getLvObj(), click_event_cb, LV_EVENT_DOUBLE_CLICKED, &counts);
        lv_obj_add_event_cb(parent->getLvObj(), click_event_cb, LV_EVENT_TRIPLE_CLICKED, &counts);
        lv_obj_add_event_cb(parent->getLvObj(), click_event_cb, LV_EVENT_LONG_PRESSED, &counts);


    }
    const cJSON* json = jsonFromConfig(config);
    if(!json) {
        LOG(WARNING, LVSIM, "Failed to parse config");
        return std::vector<std::pair<std::string, Node>>();
    }
    return leleObjectsFromJson(parent, cJSON_Print(json));
}

std::vector<std::unique_ptr<LeleStyle>> stylesFromConfig(
    const std::string &config) {

    const cJSON* json = jsonFromConfig(config);
    if(!json) {
        LOG(WARNING, LVSIM, "Failed to parse config");
        return std::vector<std::unique_ptr<LeleStyle>>();
    }
    return leleStylesFromJson(cJSON_Print(json));
}


bool parsePercentValues(const std::string &json_str, std::map<std::string, int*> &&values, const std::map<std::string, int> &&max_values) {
  bool ret = false;
  LeleWidgetFactory::fromJson(json_str, [&values, &max_values, &ret](const std::string &key, const std::string &value){
    if(key.empty()) { // e.g. json_str: "10%", so all values in the values map should get 10% of value for the given max_value[]
      if(value.size() > 1 && value.at(value.size() - 1) == '%') {
        int iret = 0;
        for(auto &[key, val]: values) {
            const auto &it = max_values.find(key);
            if(it != max_values.end()) {
                *val = std::stoi(value) * it->second / 100;
                ++iret;
            }
        }
        ret = (iret == values.size());
      }
      else if(value.size() > 0 && value.at(value.size() - 1) != '%') {
        for(auto &[key, val]: values) {
            *val = std::stoi(value);
        }
        ret = true;
      }
    }
    else {// e.g. json_str: "x:10%, y:10%", so values[x] in the values map should get 10% of value for the given max_value[]
      if(value.size() > 1 && value.at(value.size() - 1) == '%') {
        const auto &it = max_values.find(key);
        if(it != max_values.end()) {
            *values[key] = std::stoi(value) * it->second / 100;
            ret = true;
        }
      }
      else if(value.size() > 0 && value.at(value.size() - 1) != '%') {
        *values[key] = std::stoi(value);
        ret = true;
      }
    }
  });
  return ret;
}

}//LeleWidgetFactory