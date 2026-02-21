#include "lelerollerview.h"

#include <json/json_helper.h>
#include <numeric>
#include <tr/tr.h>

LOG_CATEGORY(LVSIM, "LVSIM");

LeleRollerView::LeleRollerView(LeleObject *parent, const std::string &json_str)
  : LeleObject(parent, json_str) {
  _class_name = __func__;
  fromJson(json_str);
}

bool LeleRollerView::fromJson(const std::string &json_str) {
  for (const auto &[key, token]: _nodes) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      if(key == "num_visible_items") {
        _num_visible_items = std::stoi(value);
      }
      else if(key == "items") {
        cJSONRAII json(value.c_str());
        cJSON *item = nullptr;
        cJSON_ArrayForEach(item, json()) {
          if(cJSON_IsString(item)) {
              _items.push_back(cJSON_GetStringValue(item));
          }
        }
      }
    }
  }
  return true;
}

lv_obj_t *LeleRollerView::createLvObj(LeleObject *lele_parent, lv_obj_t *lv_obj) {

  _lv_obj = LeleObject::createLvObj(lele_parent,
    lv_roller_create(lele_parent->getLvObj()));

  setItems(_items);
  lv_roller_set_visible_row_count(_lv_obj, _num_visible_items);
  lv_obj_center(_lv_obj);

  return _lv_obj;
}

bool LeleRollerView::eventCallback(LeleEvent &&e) {

  // LL(DEBUG, LVSIM) << "LeleRollerView::eventCallback " <<
  //   ", event_code: " << e.getLvEvent()->code <<
  //   " " << lv_event_code_get_name(e.getLvEvent()->code);

  switch(e.getLvEvent()->code) {
    case LV_EVENT_LONG_PRESSED:
    case LV_EVENT_LONG_PRESSED_REPEAT:
    case LV_EVENT_SCROLL: {
      break;
    }
    // case LV_EVENT_SCROLL_END:
    case LV_EVENT_CLICKED:
    case LV_EVENT_VALUE_CHANGED: 
    {
      std::string sel_value = getSelectedItem();
      LL(DEBUG, LVSIM) << "Selected value: " << sel_value.c_str();
      for(auto *py_callback:_py_callbacks) {
        if(!pyCallback(py_callback, sel_value)) {
          return false;
        }
      }
      break;
    }
    default: {
      break;
    }
  }
  return true;
}

bool LeleRollerView::pyCallback(PyObject *py_callback, const std::string &value) {

    // LOG(DEBUG, LVSIM, "LeleRollerView::pyCallback:'%p'\n", py_callback);

    PyObject *py_value = Py_BuildValue("(s)", value.c_str());
    if(!py_value) {
      LL(WARNING, LVSIM) << "LeleRollerView::pyCallback could not build py_value from string value!";
      return false;
    }
    Py_INCREF(py_value);
    return LeleObject::pyCallback(py_callback, py_value);
}

void LeleRollerView::setItems(const std::vector<std::string> &items_) {
  _items = items_;

  for(const auto &item : _items) {
    if(item.length() > _max_item_len) {
      _max_item_len = item.length();
    }
  }
  std::string items;
  items = std::accumulate(_items.begin(), _items.end(), items,
    [](const std::string& s0, const std::string& s1) {
        return s0.empty() ? s1 : s0 + "\n" + s1;
    }
  );
  // lele_set_translatable_text([this,items](){
    lv_roller_set_options(_lv_obj, tr(items).c_str(), LV_ROLLER_MODE_INFINITE);    
    // uint32_t sel_opt = lv_roller_get_selected(_lv_obj);
    // lv_roller_set_selected(_lv_obj, sel_opt, LV_ANIM_ON);
  // });
}

std::vector<std::string> LeleRollerView::getItems() const { 
  return _items; 
}

std::string LeleRollerView::getSelectedItem() const {
  std::string sel_value;
  sel_value.reserve(_max_item_len);
  int opt_id = lv_roller_get_selected(_lv_obj);
  lv_roller_get_option_str(_lv_obj, opt_id, sel_value.data(), _max_item_len);
  return sel_value;
}

void LeleRollerView::setSelectedItem(const std::string &value) {
  std::string sel_value;
  sel_value.reserve(_max_item_len);
  for(uint32_t idx = 0; idx < _items.size(); ++idx) {
    lv_roller_get_option_str(_lv_obj, idx, sel_value.data(), _max_item_len);
    if(strcmp(sel_value.c_str(), value.c_str())==0) {
      lv_roller_set_selected(_lv_obj, idx, LV_ANIM_ON);
      break;
    }
  }
}

int LeleRollerView::getSelectedItemIndex() const {
  return lv_roller_get_selected(_lv_obj);
}

void LeleRollerView::setSelectedItemIndex(int idx) {
  if(idx < 0 || idx >= _items.size()) {
    LL(WARNING, LVSIM) << "Invalid index: " << idx << " is out of range: 0-" << _items.size();
    return;
  }
  lv_roller_set_selected(_lv_obj, idx, LV_ANIM_ON);
}

void LeleRollerView::onValueChanged(PyObject *py_callback) {
  LeleObject::addEventHandler(py_callback);
  // Py_XINCREF(py_callback);
}
