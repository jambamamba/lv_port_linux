#pragma once

#include "lelebase.h"
#include "lelewidgetfactory.h"

#include <smart_pointer/auto_free_ptr.h>
#include <optional>

class LeleTabView : public LeleBase {
  public:
  class TabContent : public LeleBase {
    public:
    TabContent(const std::string &json_str);
    virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
    protected:
    std::vector<std::unique_ptr<LeleBase>> _widgets;
  };
  class TabButton : public LeleBase {
    public:
    TabButton(const std::string &json_str);
    const std::string &name() const { return _name; }
    const std::string &img() const { return _img; }
    virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
    protected:
    std::string _name;
    std::string _img;
    std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> _img_dsc;
  };
  class Tab : public LeleBase {
    public:
    Tab(const std::string &json_str = "");
    virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
    TabButton *getTabButton() const;
    TabContent *getTabContent() const;
    protected:
    std::vector<std::unique_ptr<LeleBase>> _tab_button;
    std::vector<std::unique_ptr<LeleBase>> _tab_content;
  };
  class Tabs : public LeleBase {
    public:
    Tabs(const std::string &json_str);
    virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
    int count() const;
    LeleTabView::Tab* getAt(int idx) const;
    protected:
  };
  LeleTabView(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;

  protected:
  static void tabViewDeleteEventCb(lv_event_t * e);
  lv_obj_t *setTabViewImg(lv_obj_t *tabview_header, const std::string &logo_img);
  lv_obj_t *setTabViewTitle(lv_obj_t *tabview_header, const std::string &title);
  lv_obj_t *setTabViewSubTitle(lv_obj_t *tabview_header, const std::string &subtitle);
  
  lv_style_t _style_title;
  lv_style_t _style_subtitle;
  std::string _title;
  std::string _subtitle;
  std::string _img;
  int _active_tab_bgcolor = 0xffffff;
  int _active_tab_bottom_border_color = 0;
  int _active_tab_bottom_border_width = -1;
  LeleStyle::BorderTypeE _active_tab_bottom_border_type = LeleStyle::BorderTypeE::None;
  int _tabbar_height = 75;
  Tabs *_tabs = nullptr;
  std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> _logo;
};
