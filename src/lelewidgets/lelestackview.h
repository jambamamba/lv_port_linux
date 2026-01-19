#pragma once

#include "leleobject.h"
#include "lelewidgetfactory.h"
#include <stack>

#include <utils/auto_free_ptr.h>
#include <optional>

class LeleViewHeader;
class LeleLabel;
class LeleViews;
class LeleView;
class LeleStackView : public LeleObject {
public:
  LeleStackView(const LeleObject *parent, const std::string &json_str);
  virtual bool fromJson(const std::string &json_str) override;
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  virtual bool eventCallback(LeleEvent &&e) override;

protected:
  LeleViewHeader *getViewHeader(const LeleView *view) const;
  LeleViewHeader *getBreadcrumbBar() const;
  std::vector<LeleObject*> getBreadcrumbLabels() const;
  void updateBreadcrumbLabels();
  lv_obj_t *setStackViewImg(lv_obj_t *tabview_header, const std::string &logo_img);
  lv_obj_t *setStackViewTitle(lv_obj_t *tabview_header, const std::string &title);
  lv_obj_t *setStackViewSubTitle(lv_obj_t *tabview_header, const std::string &subtitle);
  void pushView(const std::map<std::string, std::string> &args);
  void popView();

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
  LeleViews *_views = nullptr;
  std::map<std::string, std::optional<AutoFreeSharedPtr<lv_image_dsc_t>>> _images;
  std::vector<LeleView*> _stack;
};
