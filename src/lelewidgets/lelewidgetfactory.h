#pragma once

#include <debug_logger/debug_logger.h>
#include <filesystem>
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

#include "lelestyle.h"
class LeleBase;
namespace LeleWidgetFactory {

  using Token = std::variant<
    std::string, 
    std::unique_ptr<LeleBase>, 
    std::unique_ptr<LeleStyle>, 
    std::unique_ptr<LeleStyles>
  >;
  std::vector<std::pair<std::string, Token>> fromConfig(
    const std::string &config_json = std::filesystem::current_path().string() + "/config.json");
  std::vector<std::pair<std::string, Token>> fromJson(const std::string &json_str = "");

}//LeleWidgetFactory