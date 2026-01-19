#pragma once

#include <debug_logger/debug_logger.h>
#include <filesystem>
#include <functional>
#include <json_utils/json_utils.h>
#include <lvgl/lvgl_private.h>
#include <memory>
#include <optional>
#include <res/img_dsc.h>
#include <string.h>
#include <string>
#include <typeinfo>
#include <variant>
#include <vector>

class LeleEvent;
class LeleStyle;
class LeleObject;
namespace LeleWidgetFactory {
  using Node = std::variant<
    std::string
    ,std::unique_ptr<LeleObject> 
    ,std::unique_ptr<LeleEvent>
    ,std::unique_ptr<LeleStyle>
  >;
  std::vector<std::pair<std::string, Node>> fromConfig(
    LeleObject *parent,
    const std::string &config_json = std::filesystem::current_path().string() + "/config.json"
    );
  void iterateNodes(
    std::vector<std::pair<std::string, Node>> &nodes, int depth = 0, std::function<void(LeleObject &lele_object)> on_visit_lele_base_obj = nullptr);
  std::vector<std::pair<std::string, Node>> fromJson(
    const std::string &json_str = "");
  std::vector<std::pair<std::string, Node>> fromJson(
    const LeleObject *lele_obj,
    const std::string &json_str);
  void fromJson(
    const std::string &json_str, 
    std::function<void (const std::string &key, const std::string &value)> callback);
  std::vector<std::unique_ptr<LeleStyle>> stylesFromConfig(
    const std::string &config);
  bool parsePercentValues(
    const std::string &json_str, std::map<std::string, int*> &&values, const std::map<std::string, int> &&max_values = {});
  std::string trim(
    const std::string& str);
}//LeleWidgetFactory

