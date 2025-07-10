#pragma once

#include <debug_logger/debug_logger.h>
#include <json_utils/json_utils.h>
#include <lvgl/lvgl_private.h>
#include <memory>
#include <string>
#include <vector>

void addTextBox();
void addTextArea();
void addStatusMessage();
void addLoaderArc();
void addProgressBar();
void addChart();


class LeleTabView {
  public:
  LeleTabView(
    const std::string &title = "Title", 
    const std::string &subtitle = "Subtitle",
    const std::string &logo_img = "logo.png", 
    const std::vector<std::string> &tab_titles = {"Tab0", "Tab1", "Tab2"});
  lv_obj_t *obj() const { return _tab_view; }
  static std::unique_ptr<LeleTabView> fromJson(const cJSON *tabview);
  std::vector<lv_obj_t*> _tabs;

  protected:
  static void TabViewDeleteEventCb(lv_event_t * e);
  lv_style_t _style_title;
  lv_style_t _style_text_muted;
  lv_obj_t *_tab_view = nullptr;
};

class LeleLabel {
  public:
  LeleLabel(const char *text, lv_obj_t *parent = lv_screen_active(), int x = 0, int y = 0, int width = 500, int height = LV_SIZE_CONTENT, int corner_radius = 5);
  lv_obj_t *obj() const { return _text_box; }
  protected:
  lv_style_t _style;
  lv_obj_t *_text_box = nullptr;
};


class LeleTextBox {
  public:
  LeleTextBox(const std::string &text = "Textbox", lv_obj_t *parent = lv_screen_active(), int x = 0, int y = 0, int width = 500, int height = LV_SIZE_CONTENT, int corner_radius = 5);
  lv_obj_t *obj() const { return _text_area; }
  protected:
  lv_style_t _style;
  lv_obj_t *_text_area = nullptr;

  static void TextAreaEventCallback(lv_event_t * e);
};