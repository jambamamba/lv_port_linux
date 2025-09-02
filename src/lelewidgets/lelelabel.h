#pragma once

#include "lelebase.h"


class LeleLabel : public LeleBase  {
  public:
  // static std::unique_ptr<LeleLabel> fromJson(lv_obj_t *parent = lv_screen_active(), int container_width = 0, int container_height = 0, const cJSON *json = nullptr);
  // LeleLabel(lv_obj_t *parent = lv_screen_active(), const std::string &text = "Label", int x = 0, int y = 0, int width = 500, int height = LV_SIZE_CONTENT, int corner_radius = 5);
  LeleLabel(const std::string &json_str, lv_obj_t *parent = lv_screen_active());
  protected:
  lv_style_t _style;
};

