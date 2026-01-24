#include "leleview.h"

#include <lv_image_converter/lv_image_converter.h>

LOG_CATEGORY(LVSIM, "LVSIM");

////////////////////////////////////////////////////////////////////////
LeleViews::LeleViews(LeleObject *parent, const std::string &json_str)
  : LeleObject(parent, json_str) {
    _class_name = __func__ ;//
    LOG(DEBUG, LVSIM, "LeleViews json_str:%s\n", json_str.c_str());
    fromJson(json_str);
}
bool LeleViews::fromJson(const std::string &json_str) {
  return true;
}

lv_obj_t *LeleViews::createLvObj(LeleObject *lele_parent, lv_obj_t *lv_obj) {
  setParent(lele_parent);
  return _lv_obj;
}
std::vector<LeleView*> LeleViews::getChildren() const {
    std::vector<LeleView*> ret;
    for(const auto &pair: _nodes) {
      if (std::holds_alternative<std::unique_ptr<LeleObject>>(pair.second)) {
        auto &value = std::get<std::unique_ptr<LeleObject>>(pair.second);
        if(pair.first == "view") {
          LeleView *view = dynamic_cast<LeleView*> (value.get());
          if(view) {
            ret.push_back(view);
          }        
        }
      }
    }
    return ret;
}
int LeleViews::count() const {
    return getChildren().size();
}
LeleView* LeleViews::getAt(int index) const {
    auto list = getChildren();
    if(list.size() > index) {
        return list.at(index);
    }
    return nullptr;
}

void LeleViews::show(){
  for(LeleView *view : getChildren()) {
    if(view) {
      view->show();
    }
  }
}
void LeleViews::hide(){
  for(LeleView *view : getChildren()) {
    if(view) {
      view->hide();
    }
  }
}
////////////////////////////////////////////////////////////////////////
LeleViewHeader::LeleViewHeader(LeleObject *parent, const std::string &json_str)
  : LeleObject(parent, json_str) {
    _class_name = __func__ ;//
    fromJson(json_str);
}
bool LeleViewHeader::fromJson(const std::string &json_str) {
  for (const auto &[key, token]: _nodes) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "name") {
        _name = value;
      }
      else if(key == "img") {
        _img = value;
      }
    }
  }
  return true;
}
lv_obj_t *LeleViewHeader::createLvObj(LeleObject *lele_parent, lv_obj_t *lv_obj) {
  _lv_obj = LeleObject::createLvObj(lele_parent);
  for (const auto &[key, token]: _nodes) {
    if (std::holds_alternative<std::unique_ptr<LeleObject>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleObject>>(token);
      value->createLvObj(this);
    }
    else if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
    }
  }
  return _lv_obj;
}
////////////////////////////////////////////////////////////////////////


LeleView::LeleView(LeleObject *parent, const std::string &json_str)
  : LeleObject(parent, json_str) {
    _class_name = __func__ ;//
    fromJson(json_str);
}
bool LeleView::fromJson(const std::string &json_str) {
  return true;
}
lv_obj_t *LeleView::createLvObj(LeleObject *lele_parent, lv_obj_t *lv_obj) {
  _lv_obj = LeleObject::createLvObj(lele_parent);
  for (const auto &[key, token]: _nodes) {
    if (std::holds_alternative<std::unique_ptr<LeleObject>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleObject>>(token);
      value->createLvObj(this);
    }
    else if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "group") {
        _group = (value == "true");
      }
      // else if(key == "img") {
      //   std::string img_path(std::filesystem::current_path().string() + "/" + value);
      //   if(!std::filesystem::exists(img_path)) {
      //     LOG(FATAL, LVSIM, "File does not exist: '%s'\n", img_path.c_str());
      //   }
      //   // LOG(DEBUG, LVSIM, "@@@ img_path %s\n", img_path.c_str());
      //   _images[value] = LeleImageConverter::generateImgDsc(img_path.c_str());
      //   if(_images[value]) {
      //     lv_obj_t *logo = lv_image_create(_lv_obj);
      //     lv_image_set_src(logo, _images[value].value().get());
      //   }
      // }
    }
  }

  return _lv_obj;
}
bool LeleView::eventCallback(LeleEvent &&e) {
    lv_event_t* lv_event = const_cast<lv_event_t*>(e.getLvEvent());
    lv_event_code_t code = lv_event_get_code(lv_event);
    if(code != LV_EVENT_CLICKED) {
      return false;
    }
    LeleObject *base = static_cast<LeleObject*>(lv_event->user_data);
    // LOG(DEBUG, LVSIM, "%s: LeleView::eventCallback\n", base->className().c_str());
    
    //uncheck all other buttons in the group, only one button should be checked at a time
    lv_obj_t *container = (lv_obj_t *)lv_event_get_current_target(lv_event);//get the object to which an event was sent. I.e. the object whose event handler is being called.
    lv_obj_t *act_cb = lv_event_get_target_obj(lv_event);//Get the object originally targeted by the event. It's the same even if the event is bubbled. 
    if(act_cb == container) {
      // LOG(DEBUG, LVSIM, "Do nothing, the container was clicked, not the button\n");
      return LeleObject::eventCallback(std::move(e));//Do nothing if the container was clicked
    }
    for(int idx = 0; idx < lv_obj_get_child_count(container); ++idx) {
      lv_obj_t *old_cb = lv_obj_get_child(container, idx);
      lv_obj_remove_state(old_cb, LV_STATE_CHECKED);
    }
    lv_obj_add_state(act_cb, LV_STATE_CHECKED);
    _active_child_idx = lv_obj_get_index(act_cb);

    // LOG(DEBUG, LVSIM, "_lele_parent %s\n", _lele_parent->className().c_str());
    return LeleObject::eventCallback(std::move(e));
}
////////////////////////////////////////////////////////////////////////
