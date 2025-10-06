#include "leleview.h"

LOG_CATEGORY(LVSIM, "LVSIM");

////////////////////////////////////////////////////////////////////////
LeleViews::LeleViews(const std::string &json_str)
  : LeleBase(json_str) {
    _class_name = __func__ ;//
    LOG(DEBUG, LVSIM, "LeleViews json_str:%s\n", json_str.c_str());
}
lv_obj_t *LeleViews::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {
  setParent(lele_parent);
  return _lv_obj;
}
std::vector<LeleView*> LeleViews::getChildren() const {
    std::vector<LeleView*> ret;
    for(const auto &pair: _tokens) {
      if (std::holds_alternative<std::unique_ptr<LeleBase>>(pair.second)) {
        auto &value = std::get<std::unique_ptr<LeleBase>>(pair.second);
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

////////////////////////////////////////////////////////////////////////
LeleViewHeader::LeleViewHeader(const std::string &json_str)
  : LeleBase(json_str) {
    _class_name = __func__ ;//
  for (const auto &[key, token]: _tokens) {
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
}
lv_obj_t *LeleViewHeader::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {
  _lv_obj = LeleBase::createLvObj(lele_parent);
  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::unique_ptr<LeleBase>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleBase>>(token);
      value->createLvObj(this);
    }
    else if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
    }
  }
  return _lv_obj;
}
////////////////////////////////////////////////////////////////////////


LeleView::LeleView(const std::string &json_str)
  : LeleBase(json_str) {
    _class_name = __func__ ;//
}
lv_obj_t *LeleView::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {
  _lv_obj = LeleBase::createLvObj(lele_parent);
  for (const auto &[key, token]: _tokens) {
    if (std::holds_alternative<std::unique_ptr<LeleBase>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleBase>>(token);
      value->createLvObj(this);
    }
    else if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "group") {
        _group = (value == "true");
      }
    }
  }
  lv_obj_add_event_cb(_lv_obj, LeleBase::EventCallback, LV_EVENT_CLICKED, this);//also triggered when Enter key is pressed

  return _lv_obj;
}
void LeleView::hide() {
  int width = lv_obj_get_width(getLvObj());
  int height = lv_obj_get_height(getLvObj());
  lv_obj_set_size(getLvObj(), 0, 0);
}

bool LeleView::eventCallback(LeleEvent &&e) {
    lv_event_t* lv_event = const_cast<lv_event_t*>(e.lv_event());
    lv_event_code_t code = lv_event_get_code(lv_event);
    LeleBase *base = static_cast<LeleBase*>(lv_event->user_data);
    // LOG(DEBUG, LVSIM, "%s: LeleView::eventCallback\n", base->className().c_str());
    
    //uncheck all other buttons in the group, only one button should be checked at a time
    lv_obj_t *container = (lv_obj_t *)lv_event_get_current_target(lv_event);//get the object to which an event was sent. I.e. the object whose event handler is being called.
    lv_obj_t *act_cb = lv_event_get_target_obj(lv_event);//Get the object originally targeted by the event. It's the same even if the event is bubbled. 
    if(act_cb == container) {
      // LOG(DEBUG, LVSIM, "Do nothing, the container was clicked, not the button\n");
      return LeleBase::eventCallback(std::move(e));//Do nothing if the container was clicked
    }
    for(int idx = 0; idx < lv_obj_get_child_count(container); ++idx) {
      lv_obj_t *old_cb = lv_obj_get_child(container, idx);
      lv_obj_remove_state(old_cb, LV_STATE_CHECKED);
    }
    lv_obj_add_state(act_cb, LV_STATE_CHECKED);
    _active_child_idx = lv_obj_get_index(act_cb);

    // LOG(DEBUG, LVSIM, "_lele_parent %s\n", _lele_parent->className().c_str());
    return LeleBase::eventCallback(std::move(e));
}
////////////////////////////////////////////////////////////////////////
