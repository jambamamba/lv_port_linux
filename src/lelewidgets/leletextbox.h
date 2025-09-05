#pragma once

#include "lelebase.h"

class LeleTextbox : public LeleBase  {
  public:
  // static std::unique_ptr<LeleTextbox> fromJson(lv_obj_t *parent = lv_screen_active(), int container_width = 0, int container_height = 0, const cJSON *json = nullptr);
  // LeleTextbox(lv_obj_t *parent = lv_screen_active(), const std::string &text = "Textbox", int x = 0, int y = 0, int width = 500, int height = LV_SIZE_CONTENT, int corner_radius = 5);
  LeleTextbox(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr) override;
  protected:
  static void TextAreaEventCallback(lv_event_t * e);
  std::string _text;
  int _max_length = 15;
  bool _multiline = false;
};