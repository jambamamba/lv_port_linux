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

extern std::filesystem::path applicationPath();

class LeleEvent;
class LeleStyle;
class LeleBase;
namespace LeleWidgetFactory {
  using Token = std::variant<
    std::string
    ,std::unique_ptr<LeleBase> 
    ,std::unique_ptr<LeleEvent>
    ,std::unique_ptr<LeleStyle>
  >;
  std::vector<std::pair<std::string, Token>> fromConfig(
    const std::string &config_json = std::filesystem::current_path().string() + "/config.json");
  void fromJson(const std::string &json_str, std::function<void (const std::string &key, const std::string &value)> callback);
  std::vector<std::pair<std::string, Token>> fromJson(const std::string &json_str = "");
  bool parseXY(const std::string &value, const std:vector<int> &names, std::vector<int*> &values, const std::vector<int> &max_val = {});
}//LeleWidgetFactory