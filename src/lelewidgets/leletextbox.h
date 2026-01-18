#pragma once

#include "leleobject.h"

class LeleTextbox : public LeleObject  {
public:
  LeleTextbox(const std::string &json_str = nullptr);
  virtual bool fromJson(const std::string &json_str, const LeleObject *parent) override;
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  virtual bool eventCallback(LeleEvent &&e) override;
protected:
  std::string _text;
  int _max_length = 15;
  bool _multiline = false;
  lv_text_align_t _text_align = LV_TEXT_ALIGN_AUTO;
};