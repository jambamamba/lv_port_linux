#include "lelemessagebox.h"

#include "leleevent.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleMessageBox::LeleMessageBox(const std::string &json_str)
  : LeleBase(json_str) {

  _class_name = __func__ ;//typeid(this).name();
  for (const auto &[key, token]: _nodes) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "title") {
        _title = value;
      }
      else if(key == "text") {
        _text = value;
      }
    }
    else if(std::holds_alternative<std::unique_ptr<LeleEvent>>(token)) {
      LeleEvent *event = std::get<std::unique_ptr<LeleEvent>>(token).get();
      _events.push_back(event);
    }
    // else if (std::holds_alternative<std::unique_ptr<LeleBase>>(token)) {
    //   auto &value = std::get<std::unique_ptr<LeleBase>>(token);
    //   value->createLvObj(this);//osm
    //   // if(key == "button") {
    //   //   _btn = std::make_unique<LeleButtons::LeleButton>(value);
    //   // }
    // }
  }
}

lv_obj_t *LeleMessageBox::createLvObj(LeleBase *lele_parent, lv_obj_t *lv_obj) {

  _lv_obj = LeleBase::createLvObj(lele_parent,
    lv_msgbox_create(nullptr));

  lv_msgbox_add_title(_lv_obj, _title.c_str());
  lv_msgbox_add_text(_lv_obj, _text.c_str());

  for (const auto &[key, token]: _nodes) {
    if (std::holds_alternative<std::unique_ptr<LeleBase>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleBase>>(token);
      LeleButtons::LeleButton *lele_btn = dynamic_cast<LeleButtons::LeleButton *>(value.get());
      if(!lele_btn) {
        continue;
      }
      else if(lele_btn->type() == LeleButtons::LeleButton::Type::Close) {
        lv_obj_t *lv_btn = lv_msgbox_add_header_button(_lv_obj, LV_SYMBOL_CLOSE);
        lele_btn->setStyle(lv_btn);
        lv_obj_add_event_cb(lv_btn, [](lv_event_t *e){
          lv_obj_t *btn = static_cast<lv_obj_t *>(lv_event_get_current_target(e));
          lv_obj_t *mbox = lv_obj_get_parent(lv_obj_get_parent(btn));
          lv_msgbox_close(mbox);
        }, LV_EVENT_CLICKED, lele_btn);
      }
      else if(lele_btn->type() == LeleButtons::LeleButton::Type::Push) {
        lv_obj_t *lv_btn = lv_msgbox_add_footer_button(_lv_obj, lele_btn->text().c_str());
        lele_btn->setStyle(lv_btn);
        lv_obj_add_event_cb(lv_btn, [](lv_event_t *e){
          lv_obj_t *btn = static_cast<lv_obj_t *>(lv_event_get_current_target(e));
          lv_obj_t *mbox = lv_obj_get_parent(lv_obj_get_parent(btn));
        }, LV_EVENT_CLICKED, lele_btn);
      }
    }
  }
  return _lv_obj;
}

void LeleMessageBox::setText(const std::string &text) {
  _text = text;
  lv_label_set_text(_lv_obj, _text.c_str());
}

std::string LeleMessageBox::getText() const { 
  return _text; 
}

bool LeleMessageBox::eventCallback(LeleEvent &&e) {
    lv_event_t* lv_event = const_cast<lv_event_t*>(e.lv_event());
    lv_event_code_t code = lv_event_get_code(lv_event);

    if(code == LV_EVENT_CLICKED) {
        // LOG(DEBUG, LVSIM, "%s: clicked. button type:%i\n", _class_name.c_str(), _type);
        for(LeleEvent *event: _events) {
          if(event->type() == "clicked"){
            // e->copy(event.id(), event->type(), event->action(), event->args);
            // LOG(DEBUG, LVSIM, "LeleButtons::LeleButton::eventCallback\n");
            return LeleBase::eventCallback(LeleEvent(*event, lv_event));
          }
        }
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LOG(DEBUG, LVSIM, "%s: value changed\n", _class_name.c_str());
    }
    return LeleBase::eventCallback(std::move(e));
}
