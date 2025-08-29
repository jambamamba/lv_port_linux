#pragma once

#include "lelebase.h"


class LeleTextBox : public LeleBase  {
  public:
  // static std::unique_ptr<LeleTextBox> fromJson(lv_obj_t *parent = lv_screen_active(), int container_width = 0, int container_height = 0, const cJSON *json = nullptr);
  LeleTextBox(lv_obj_t *parent = lv_screen_active(), const std::string &text = "Textbox", int x = 0, int y = 0, int width = 500, int height = LV_SIZE_CONTENT, int corner_radius = 5);
  protected:
  lv_style_t _style;

  static void TextAreaEventCallback(lv_event_t * e);
};