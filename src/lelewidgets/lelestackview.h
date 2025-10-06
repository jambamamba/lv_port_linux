#pragma once

#include "lelebase.h"
#include "lelewidgetfactory.h"

#include <smart_pointer/auto_free_ptr.h>
#include <optional>

class LeleViewHeader;
class LeleViews;
class LeleStackView : public LeleBase {
  public:
  LeleStackView(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  virtual bool eventCallback(LeleEvent &&e) override;

  protected:
  LeleViewHeader *getButtonBar() const;
  lv_obj_t *setStackViewImg(lv_obj_t *tabview_header, const std::string &logo_img);
  lv_obj_t *setStackViewTitle(lv_obj_t *tabview_header, const std::string &title);
  lv_obj_t *setStackViewSubTitle(lv_obj_t *tabview_header, const std::string &subtitle);
  
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
  std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> _logo;
};
