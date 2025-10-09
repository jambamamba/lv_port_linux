#include <iostream>

#include "lelestackview.h"

#include "lelelabel.h"
#include "leletextbox.h"
#include "lelestyle.h"
#include "leleview.h"

#include <lv_image_converter/mainlib.h>

LOG_CATEGORY(LVSIM, "LVSIM");

LeleStackView::LeleStackView(const std::string &json_str)
  : LeleBase(json_str) {
    _class_name = __func__ ;//
  for (const auto &[key, token]: _tokens) {
    LOG(DEBUG, LVSIM, "Process token with key: %s\n", key.c_str());
    if (std::holds_alternative<std::unique_ptr<LeleBase>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleBase>>(token);
      if(key == "views") {
          _views = dynamic_cast<LeleViews*> (value.get());
      }
    }
    else if (std::holds_alternative<std::string>(token)) {
      std::string value = std::get<std::string>(token);
      if(key == "title") {
        _title = value;
      }
      else if(key == "subtitle") {
        _subtitle = value;
      }
      else if(key == "img") {
        _img = value;
      }
      else if(key == "active_tab_bgcolor") {
        _active_tab_bgcolor = LeleStyle::parseColorCode(value);
      }
      else if(key == "active_tab_bottom_border") {
        std::tie(_active_tab_bottom_border_type,_active_tab_bottom_border_width,_active_tab_bottom_border_color) = 
          LeleStyle::parseBorder(value);
        // LOG(DEBUG, LVSIM, "value:%s\n", value.c_str());
        // LOG(DEBUG, LVSIM, "_active_tab_bottom_border_type:%i\n",_active_tab_bottom_border_type);
        // LOG(DEBUG, LVSIM, "_active_tab_bottom_border_width:%i\n",_active_tab_bottom_border_width);
        // LOG(DEBUG, LVSIM, "_active_tab_bottom_border_color:0x%x\n",_active_tab_bottom_border_color);
      }
      else if(key == "tabbar_height") {
        _tabbar_height = std::stoi(value.c_str(), nullptr, 10);
      }
    }
  }
  std::cout << "StackView: styles: " << _lele_styles << "\n";
}

static void event_cb1(lv_event_t * e) {
  LOG(DEBUG, LVSIM, "@@@@@@@ %s:%s\n", __FUNCTION__, (char*)e->user_data);
}

LeleViewHeader *LeleStackView::getViewHeader(const LeleView *view) const {
  std::vector<LeleBase *> objs = view->getLeleObj("view_header");
  if(objs.size() > 0) {
    return dynamic_cast<LeleViewHeader*>(objs.front());
  }
  return nullptr;
}

LeleViewHeader *LeleStackView::getBreadcrumbBar() const {
  std::vector<LeleBase *> objs = getLeleObj("view_header");
  if(objs.size() > 0) {
    return dynamic_cast<LeleViewHeader*>(objs.front());
  }
  return nullptr;
}

std::vector<LeleBase*> LeleStackView::getBreadcrumbLabels() const {
  LeleViewHeader *breadcrumb_bar = LeleStackView::getBreadcrumbBar();
  std::vector<LeleBase *> objs = breadcrumb_bar->getLeleObj("label");
  return objs;
}

void LeleStackView::updateBreadcrumbLabels() {
  std::vector<LeleBase*> labels = getBreadcrumbLabels();
  auto label_it = labels.begin();
  auto stack_it = _stack.begin();
  if(_stack.size() >= labels.size()) {
    for(int idx = 0; idx < _stack.size() - labels.size(); ++idx) {
      ++stack_it;
    }
    LeleLabel *label = dynamic_cast<LeleLabel *>(*label_it);
    if(_stack.size() > labels.size()){
      ++stack_it;
      label->setText("..");
      ++label_it;
    }
    else {
      const LeleView *view = dynamic_cast<const LeleView *>(*stack_it);
      ++stack_it;
      LeleViewHeader *view_header = getViewHeader(view);
      label->setText(view_header->name());
      ++label_it;
    }
    for (; label_it != labels.end() && stack_it != _stack.end(); ++label_it, ++stack_it) { 
      LeleLabel *label = dynamic_cast<LeleLabel *>(*label_it);
      const LeleView *view = dynamic_cast<const LeleView *>(*stack_it);
      LeleViewHeader *view_header = getViewHeader(view);
      if(view_header) {
        label->setText(view_header->name());
      }
    } 
  }
  else {
    for (; label_it != labels.end() && stack_it != _stack.end(); ++label_it, ++stack_it) { 
      LeleLabel *label = dynamic_cast<LeleLabel *>(*label_it);
      const LeleView *view = dynamic_cast<const LeleView *>(*stack_it);
      LeleViewHeader *view_header = getViewHeader(view);
      if(view_header) {
        label->setText(view_header->name());
      }
    }
    for (; label_it != labels.end(); ++label_it) { 
      LeleLabel *label = dynamic_cast<LeleLabel *>(*label_it);
      label->setText("");
    }
  }
}

lv_obj_t *LeleStackView::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {

  _lv_obj = LeleBase::createLvObj(lele_parent);
  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::unique_ptr<LeleBase>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleBase>>(token);
      value->createLvObj(this);//osm
      // LOG(DEBUG, LVSIM, "stackview obj: %s:%s\n", key.c_str(), value->id().c_str());
    }
    else if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      // LOG(DEBUG, LVSIM, "stackview obj: %s:%s\n", key.c_str(), value.c_str());
    }
  }
  lv_obj_set_size(_lv_obj, lv_pct(100), lv_pct(100));

  if(_views) {
    _views->createLvObj(this);
    _views->setLvObj(_lv_obj);
    for(int idx = 0; idx < _views->count(); ++idx) {
      LeleView *view = _views->getAt(idx);
      view->createLvObj(_views);
      if(idx == 0) {
        _stack.push_back(view);
      }
      else {
        view->hide();
      }
    }
    updateBreadcrumbLabels();
  }

  // lv_obj_t *logo = setStackViewImg(tab_bar, _img);
  // lv_obj_t *label = setStackViewTitle(tab_bar, _title);
  // lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);
  // label = setStackViewSubTitle(tab_bar, _subtitle);
  // lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_BOTTOM, 10, 0);

  return _lv_obj;
}

lv_obj_t *LeleStackView::setStackViewImg(lv_obj_t *tabview_header, const std::string &img) {//osm: same as setTabViewImg
    lv_obj_set_style_pad_left(tabview_header, LV_HOR_RES / 2, 0);
    lv_obj_t *logo = lv_image_create(tabview_header);
    lv_obj_add_flag(logo, LV_OBJ_FLAG_IGNORE_LAYOUT);
    // lv_image_set_src(logo, _lv_img_dsc_map.at(img));
    _logo = generateImgDsc((std::string("/repos/lv_port_linux/res/") + img).c_str());//osm
    if(_logo) {
      lv_image_set_src(logo, _logo.value().get());
    }
    lv_obj_align(logo, LV_ALIGN_LEFT_MID, -LV_HOR_RES / 2 + 25, 0);
    return logo;
}

lv_obj_t *LeleStackView::setStackViewTitle(lv_obj_t *tabview_header, const std::string &title) {//osm: same as setTabViewTitle
    lv_obj_t *label = lv_label_create(tabview_header);
    lv_style_init(&_style_title);
    const lv_font_t *font_large = &lv_font_montserrat_24;
    lv_style_set_text_font(&_style_title, font_large);
    lv_obj_add_style(label, &_style_title, LV_PART_MAIN);
    lv_obj_add_flag(label, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_label_set_text_fmt(label, "%s", title.c_str());//"LVGL v%d.%d.%d", lv_version_major(), lv_version_minor(), lv_version_patch());
    return label;
}

lv_obj_t *LeleStackView::setStackViewSubTitle(lv_obj_t *tabview_header, const std::string &subtitle) {//osm: same as setTabViewSubTitle
    lv_obj_t *label = lv_label_create(tabview_header);
    lv_style_init(&_style_subtitle);
    lv_style_set_text_opa(&_style_subtitle, LV_OPA_50);
    lv_obj_add_style(label, &_style_subtitle, LV_PART_MAIN);
    lv_obj_add_flag(label, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_label_set_text(label, subtitle.c_str());
    return label;
}

void LeleStackView::pushView(const std::map<std::string, std::string> &args) {
  for(auto &[name,id]: args) {
    LOG(DEBUG, LVSIM, "LeleStackView::eventCallback arg: name:%s, id:%s\n", name.c_str(), id.c_str());
    if(name == "id") {
      //osm todo:  update breadcrumbs, X button should be right aligned
      for(LeleView *view : _views->getChildren()) {
        if(view->id() == id) {
          view->show();
          _stack.push_back(view);
        }
        else {
          view->hide();
        }
      }
      updateBreadcrumbLabels();
    }
  }
}

void LeleStackView::popView() {
  if(_stack.size() < 2) {
    return;
  }
  _views->hide();
  _stack.pop_back();
  _stack.back()->show();
  updateBreadcrumbLabels();
}

bool LeleStackView::eventCallback(LeleEvent &&e) {
  if(e.type() == "clicked") {
    LOG(DEBUG, LVSIM, "LeleStackView::eventCallback, type:%s, action:%s\n", e.type().c_str(), e.action().c_str());
    if(e.action() == "stackview.push"){
      pushView(e.args());
    }
    else if(e.action() == "stackview.pop"){
      popView();
    }
  }
  return LeleBase::eventCallback(std::move(e));
}