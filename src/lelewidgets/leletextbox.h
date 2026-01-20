#pragma once

#include "leleobject.h"

class LeleTextbox : public LeleObject  {
public:
  // static std::unique_ptr<LeleTextbox> fromJson(lv_obj_t *parent = lv_screen_active(), int container_width = 0, int container_height = 0, const cJSON *json = nullptr);
  // LeleTextbox(lv_obj_t *parent = lv_screen_active(), const std::string &text = "Textbox", int x = 0, int y = 0, int width = 500, int height = LV_SIZE_CONTENT, int corner_radius = 5);
  LeleTextbox(LeleObject *parent, const std::string &json_str);
  virtual bool fromJson(const std::string &json_str) override;
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  virtual bool eventCallback(LeleEvent &&e) override;
protected:
  std::string _text;
  int _max_length = 15;
  bool _multiline = false;
  lv_text_align_t _text_align = LV_TEXT_ALIGN_AUTO;
};