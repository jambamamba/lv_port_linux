#pragma once

#include <functional>
#include <optional>
#include <lvgl/lvgl_private.h>
#include <map>
#include <string>
#include <utils/auto_free_ptr.h>
#include <lelewidgets/lelestyle.h>

namespace ImageBuilder {


    int getParentDimension(const std::string &key, const LeleObject *lele_obj);
    int parseColorCode(const std::string &color_str);
    std::pair<std::map<std::string, std::optional<LeleStyle::StyleValue>>, std::vector<std::string>>
        parseBackground(const std::string &key, const std::string &value_, LeleObject *lele_obj);
    std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> fillBackgroundColor(int color, int obj_width, int obj_height);
    std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> drawBackgroundImage(
        const std::string &prefix,
        const std::string &src, 
        const std::vector<std::string> &style_keys,
        const std::map<std::string, std::optional<LeleStyle::StyleValue>> &style_map,
        int obj_width, 
        int obj_height);

};