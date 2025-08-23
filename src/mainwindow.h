#pragma once

#include <debug_logger/debug_logger.h>
#include <json_utils/json_utils.h>
#include <lvgl/lvgl_private.h>
#include <memory>
#include <res/img_dsc.h>
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
  class Tab {
    public:
      static Tab fromJson(const cJSON *tab);
      Tab(const std::string &title = "Tab", const std::string &img = "", const std::string &content = "")
      : _title(title)
      , _img(img)
      , _content(content)
      {};
      const std::string &title() const {
        return _title;
      } 
      const std::string &img() const {
        return _img;
      }
      const std::string &content() const {
        return _content;
      }
      lv_obj_t *getLvObj() const {
        return _lv_obj;
      }
      void setLvObj(lv_obj_t *obj) {
        _lv_obj = obj;
      }
      void setTabButton(lv_obj_t *button, int active_tab_bgcolor, int active_tab_bottom_border_color);
      void loadTabContent();
    protected:
      std::string _title;
      std::string _img;
      std::string _content;
      lv_obj_t *_lv_obj = nullptr;
      lv_obj_t *_tab_button = nullptr;
  };
  LeleTabView(
    const std::string &title = "Title", 
    const std::string &subtitle = "Subtitle",
    const std::string &logo_img = "logo.png",
    const std::string &fgcolor = "#ffffff",
    const std::string &bgcolor = "#444444",
    const std::string &active_tab_bgcolor_str = "#ffffff",
    const std::string &active_tab_bottom_border_color_str = "#121212",
    const std::vector<Tab> &tabs = {{"Tab0"}, {"Tab1"}, {"Tab2"}}
  );
  lv_obj_t *getLvObj() const {
    return _lv_obj;
  }
  static std::unique_ptr<LeleTabView> fromJson(const cJSON *tabview);
  std::vector<Tab> _tabs;

  protected:
  static void tabViewDeleteEventCb(lv_event_t * e);
  lv_obj_t *setTabViewImg(lv_obj_t *tabview_header, const std::string &logo_img);
  lv_obj_t *setTabViewTitle(lv_obj_t *tabview_header, const std::string &title);
  lv_obj_t *setTabViewSubTitle(lv_obj_t *tabview_header, const std::string &subtitle);
  lv_style_t _style_title;
  lv_style_t _style_text_muted;
  lv_obj_t *_lv_obj = nullptr;
  const std::string _bgcolor = "#ffffff";
};

class LelePos {
  public:
  static LelePos fromJson(int parent_width = 0, int parent_height = 0, const cJSON *json = nullptr);
  LelePos(int parent_width = 0, int parent_height = 0, const std::string &x = "", const std::string &y = "", const std::string &width = "", const std::string &height = "");
  int x() const;
  int y() const;
  int width() const;
  int height() const;
  protected:
  int absFromPercent(int percent, int parent) const;
  std::string _x = "0";
  std::string _y = "0";
  std::string _width = "0";
  std::string _height = "0";
  int _parent_width = 0;
  int _parent_height = 0;
};

class LeleLabel {
  public:
  static LeleLabel fromJson(lv_obj_t *parent = lv_screen_active(), const cJSON *json = nullptr);
  LeleLabel(lv_obj_t *parent = lv_screen_active(), const std::string &text = "Label", int x = 0, int y = 0, int width = 500, int height = LV_SIZE_CONTENT, int corner_radius = 5);
  lv_obj_t *obj() const { return _text_box; }
  protected:
  lv_style_t _style;
  lv_obj_t *_text_box = nullptr;
};


class LeleTextBox {
  public:
  static LeleTextBox fromJson(lv_obj_t *parent = lv_screen_active(), const cJSON *json = nullptr);
  LeleTextBox(lv_obj_t *parent = lv_screen_active(), const std::string &text = "Textbox", int x = 0, int y = 0, int width = 500, int height = LV_SIZE_CONTENT, int corner_radius = 5);
  lv_obj_t *obj() const { return _text_area; }
  protected:
  lv_style_t _style;
  lv_obj_t *_text_area = nullptr;

  static void TextAreaEventCallback(lv_event_t * e);
};