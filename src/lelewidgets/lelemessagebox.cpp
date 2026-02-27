#include "lelemessagebox.h"

#include <tr/tr.h>

#include "leleevent.h"

LOG_CATEGORY(LVSIM, "LVSIM");

LeleMessageBox::LeleMessageBox(LeleObject *parent, const std::string &json_str)
  : LeleLabel(parent, json_str) {

  _type = __func__ ;//typeid(this).name();
  fromJson(json_str);
}
bool LeleMessageBox::fromJson(const std::string &json_str) {
  for (const auto &[key, token]: _nodes) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "title") {
        _title = value;
      }
    }
    else if(std::holds_alternative<std::unique_ptr<LeleEvent>>(token)) {
      LeleEvent *event = std::get<std::unique_ptr<LeleEvent>>(token).get();
      _events.push_back(event);
    }
  }
  return true;
}
lv_obj_t *LeleMessageBox::createLvObj(LeleObject *lele_parent, lv_obj_t *lv_obj) {

  _lv_obj = LeleObject::createLvObj(lele_parent,
    lv_msgbox_create(nullptr));

  _lv_title = lv_msgbox_add_title(_lv_obj, _title.c_str());
  // lv_msgbox_add_text(_lv_obj, tr(_text).c_str());

  for (const auto &[key, token]: _nodes) {
    if (std::holds_alternative<std::unique_ptr<LeleObject>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleObject>>(token);
      LeleButtons::LeleButton *lele_btn = dynamic_cast<LeleButtons::LeleButton *>(value.get());
      if(!lele_btn) {
        continue;
      }
      else if(lele_btn->getType() == LeleButtons::LeleButton::Type::Close) {
        addEventCallback(lele_btn, lv_msgbox_add_header_button(_lv_obj, LV_SYMBOL_CLOSE));
      }
      else if(lele_btn->getType() == LeleButtons::LeleButton::Type::Push) {
        addEventCallback(lele_btn, lv_msgbox_add_footer_button(_lv_obj, lele_btn->getText().c_str()));
      }
    }
  }
  return _lv_obj;
}

void LeleMessageBox::addEventCallback(LeleButtons::LeleButton *lele_btn, lv_obj_t *lv_btn) const {
  lele_btn->setLvObj(lv_btn);
  lele_btn->applyStyle(lv_btn);
  lv_obj_add_event_cb(lv_btn, [](lv_event_t *e){
    lv_obj_t *btn = static_cast<lv_obj_t *>(lv_event_get_current_target(e));
    lv_obj_t *mbox = lv_obj_get_parent(lv_obj_get_parent(btn));
    LeleMessageBox *self = static_cast<LeleMessageBox*>(mbox->user_data);
    self->_btn_clicked = static_cast<LeleButtons::LeleButton *>(btn->user_data);
    EventCallback(e);
  }, LV_EVENT_CLICKED, const_cast<LeleMessageBox *>(this));
}

void LeleMessageBox::setTitle(const std::string &text) {
  lv_label_set_text(_lv_title, text.c_str());
  _title = text;
}

std::string LeleMessageBox::getTitle() const { 
  _title = lv_label_get_text(_lv_title);
  return _title;
}

LeleButtons::LeleButton *LeleMessageBox::getButtonClicked() const { 
  return _btn_clicked;
}

bool LeleMessageBox::eventCallback(LeleEvent &&e) {
    LOG(DEBUG, LVSIM, "LeleMessageBox::eventCallback, class:%s, btn_clicked.id:%s\n", 
      _type.c_str(), _btn_clicked ? _btn_clicked->getId().c_str() : "");
    lv_event_t* lv_event = const_cast<lv_event_t*>(e.getLvEvent());
    lv_event_code_t code = lv_event_get_code(lv_event);

    if(code == LV_EVENT_CLICKED) {
        LOG(DEBUG, LVSIM, "%s: clicked\n", _type.c_str());
        for(LeleEvent *event: _events) {
          if(event->getType() == "clicked"){
            event->setTargetObj(e.getTargetObj());
            if(!LeleObject::eventCallback(LeleEvent(*event, lv_event))) {
              return false;
            }
          }
        }
    }
    if(!LeleObject::eventCallback(std::move(e))) {
      return false;
    }
    if(e.getTargetObj()) {
      lv_msgbox_close(e.getTargetObj()->getLvObj());
    }
    return true;
}
