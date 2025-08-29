#pragma once

#include "lelebase.h"

class LeleTabView : public LeleBase {
  public:
  class Tab : public LeleBase {
    public:
      static std::unique_ptr<Tab> fromJson(const cJSON *tab);
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
      void setTabButton(lv_obj_t *button, int active_tab_bgcolor, int active_tab_bottom_border_color);
      void setTabContent(lv_obj_t *content);
    protected:
      std::string _title;
      std::string _img;
      std::string _content;
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
    std::vector<std::unique_ptr<Tab>> &&tabs = {std::make_unique<Tab>("Tab0"), std::make_unique<Tab>("Tab1"), std::make_unique<Tab>("Tab2")}
  );
  static std::optional<std::unique_ptr<LeleTabView>> fromJson(const cJSON *tabview);
  std::vector<std::unique_ptr<Tab>> _tabs;

  protected:
  static void tabViewDeleteEventCb(lv_event_t * e);
  lv_obj_t *setTabViewImg(lv_obj_t *tabview_header, const std::string &logo_img);
  lv_obj_t *setTabViewTitle(lv_obj_t *tabview_header, const std::string &title);
  lv_obj_t *setTabViewSubTitle(lv_obj_t *tabview_header, const std::string &subtitle);
  lv_style_t _style_title;
  lv_style_t _style_text_muted;
  const std::string _bgcolor = "#ffffff";
};
