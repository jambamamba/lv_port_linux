#pragma once

#include "leleobject.h"

#include <smart_pointer/auto_free_ptr.h>

class LeleView : public LeleObject {
public:
  LeleView(const std::string &json_str);
  virtual bool fromJson(const std::string &json_str) override;
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  virtual bool eventCallback(LeleEvent &&e) override;
  bool isGroup() const { return _group; }
protected:
  int _active_child_idx = -1;
  bool _group = false;
  std::map<std::string, std::optional<AutoFreeSharedPtr<lv_image_dsc_t>>> _images;
};
class LeleViewHeader : public LeleObject {
public:
  LeleViewHeader(const std::string &json_str);
  virtual bool fromJson(const std::string &json_str) override;
  const std::string &name() const { return _name; }
  const std::string &img() const { return _img; }
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
protected:
  std::string _name;
  std::string _img;
  std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> _img_dsc;
};
class LeleViews : public LeleObject {
public:
  LeleViews(const std::string &json_str);
  virtual bool fromJson(const std::string &json_str) override;
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  int count() const;
  LeleView* getAt(int idx) const;
  std::vector<LeleView*> getChildren() const;
  virtual void show() override;
  virtual void hide() override;
protected:
};