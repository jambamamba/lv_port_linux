#pragma once

#include "lelebase.h"

class LeleView : public LeleBase {
  public:
  LeleView(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  virtual void eventCallback(lv_event_t * e) override;
  bool isGroup() const { return _group; }
  protected:
  int _active_child_idx = -1;
  bool _group = false;
};

class LeleViews : public LeleBase {
  public:
  LeleViews(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleBase *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  int count() const;
  LeleView* getAt(int idx) const;
  std::vector<LeleView*> getChildren() const;
  protected:
};