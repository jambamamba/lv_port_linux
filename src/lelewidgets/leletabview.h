#pragma once

#include "leleobject.h"
#include "lelewidgetfactory.h"

#include <smart_pointer/auto_free_ptr.h>
#include <optional>

class LeleViewHeader;
class LeleTabView : public LeleObject {
  public:
  class TabHeader : public LeleObject {
    public:
    TabHeader(const std::string &json_str);
    const std::string &name() const { return _name; }
    const std::string &img() const { return _img; }
    virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
    protected:
    std::string _name;
    std::string _img;
    std::map<std::string, std::optional<AutoFreeSharedPtr<lv_image_dsc_t>>> _images;
  };
  class TabContent : public LeleObject {
    public:
    TabContent(const std::string &json_str);
    virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
    protected:
    std::vector<std::unique_ptr<LeleObject>> _widgets;
  };
  class Tab : public LeleObject {
    public:
    Tab(const std::string &json_str = "");
    virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
    TabHeader *getTabHeader() const;
    TabContent *getTabContent() const;
    protected:
    std::vector<std::unique_ptr<LeleObject>> _tab_button;
    std::vector<std::unique_ptr<LeleObject>> _tab_content;
  };
  class Tabs : public LeleObject {
    public:
    Tabs(const std::string &json_str);
    virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
    int count() const;
    LeleTabView::Tab* getAt(int idx) const;
    std::vector<LeleTabView::Tab*> getChildren() const;
    protected:
  };
  LeleTabView(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;

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
  enum TabBarLocationE {
    Top,
    Bottom
  };
  TabBarLocationE _tabbar_location = TabBarLocationE::Top;
  Tabs *_tabs = nullptr;
  std::map<std::string, std::optional<AutoFreeSharedPtr<lv_image_dsc_t>>> _images;
};
