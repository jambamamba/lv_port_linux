#include "lelebase.h"

LeleBase::LeleBase(const std::string &json_str, lv_obj_t *parent)
: _lv_parent_obj(parent) {
    _tokens = LeleWidgetFactory::fromJson(json_str);
}
