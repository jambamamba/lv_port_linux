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


class LeleBase;
namespace LeleWidgetFactory {

  // struct FactoryInput {
  //   LelePos _pos;
  //   std::string _text, _title, _subtitle, _name, _img, _fgcolor, _bgcolor, 
  //       _active_tab_bgcolor, _active_tab_bottom_border_color, _json_str;
  //   std::vector<std::pair<std::string /*widget_type*/, std::string /*json_str*/>> _widgets;
  //   // mutable std::vector<std::unique_ptr<LeleBase>> _tabs;

  //   FactoryInput(
  //       const std::string &json_str,
  //       int container_width = lv_obj_get_width(lv_screen_active()),
  //       int container_height = lv_obj_get_height(lv_screen_active())
  //   );
  // };
  // std::vector<std::unique_ptr<LeleBase>> createLeleWidget(
  //   const std::string &json_str,
  //   lv_obj_t *parent = lv_screen_active(),
  //   int container_width = lv_obj_get_width(lv_screen_active()),
  //   int container_height = lv_obj_get_height(lv_screen_active())
  // );
  using Token = std::variant<std::string, std::unique_ptr<LeleBase>>;
  std::vector<std::pair<std::string, Token>> fromConfig(
    const std::string &config_json = std::filesystem::current_path().string() + "/config.json");
  std::vector<std::pair<std::string, Token>> fromJson(
    const std::string &json_str = "", lv_obj_t *parent = lv_screen_active());

}//LeleWidgetFactory