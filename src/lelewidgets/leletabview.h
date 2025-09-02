#pragma once

#include "lelebase.h"
#include "lelewidgetfactory.h"

class LeleTabView : public LeleBase {
  public:
  class TabContent : public LeleBase {
    public:
    TabContent(const std::string &json_str, lv_obj_t *parent = lv_screen_active());
    protected:
    std::vector<std::unique_ptr<LeleBase>> _widgets;
  };
  class TabButton : public LeleBase {
    public:
    TabButton(const std::string &json_str, lv_obj_t *parent = lv_screen_active());
    protected:
    std::string _name;
    std::string _img;
  };
  class Tab : public LeleBase {
    public:
    Tab(const std::string &json_str, lv_obj_t *parent = lv_screen_active());
    protected:
    std::vector<std::unique_ptr<LeleBase>> _tab_button;
    std::vector<std::unique_ptr<LeleBase>> _tab_content;
  };
  class Tabs : public LeleBase {
    public:
    Tabs(const std::string &json_str, lv_obj_t *parent = lv_screen_active());
    protected:
    std::vector<std::unique_ptr<LeleBase>> _tab;
  }; 
  // class Tab : public LeleBase {
  //   public:
  //     Tab(const std::vector<std::string> &json_str = {""})
  //     : _json_str(json_str) 
  //     {
  //       // auto tab_obj = fromJson(tab_json_str);
  //       // auto *tab = dynamic_cast<LeleTabView::Tab*>(tab_obj.get());
  //     }
  //     const std::string &title() const {
  //       return _title;
  //     } 
  //     const std::string &img() const {
  //       return _img;
  //     }
  //     void setTabButton(lv_obj_t *button, int active_tab_bgcolor, int active_tab_bottom_border_color);
  //     void setTabContent(lv_obj_t *tab_content);
  //   protected:
  //     std::string _title;
  //     std::string _img;
  //     std::vector<std::string> _json_str;
  //     lv_obj_t *_tab_button = nullptr;
  // };
  // LeleTabView(
  //   const std::string &title = "Title", 
  //   const std::string &subtitle = "Subtitle",
  //   const std::string &logo_img = "logo.png",
  //   const std::string &fgcolor = "#ffffff",
  //   const std::string &bgcolor = "#444444",
  //   const std::string &active_tab_bgcolor_str = "#ffffff",
  //   const std::string &active_tab_bottom_border_color_str = "#121212",
  //   const std::vector<std::string> &tabs_json_str = {""}
  // );
  LeleTabView(const std::string &json_str, lv_obj_t *parent = lv_screen_active());
  // static std::optional<std::unique_ptr<LeleTabView>> fromJson(const cJSON *tabview);

  protected:
  std::vector<std::unique_ptr<LeleBase>> _tabs;
  static void tabViewDeleteEventCb(lv_event_t * e);
  lv_obj_t *setTabViewImg(lv_obj_t *tabview_header, const std::string &logo_img);
  lv_obj_t *setTabViewTitle(lv_obj_t *tabview_header, const std::string &title);
  lv_obj_t *setTabViewSubTitle(lv_obj_t *tabview_header, const std::string &subtitle);
  lv_style_t _style_title;
  lv_style_t _style_subtitle;
  const std::string _bgcolor = "#ffffff";
};
