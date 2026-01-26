#pragma once

#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <optional>
#include <utils/auto_free_ptr.h>
#include <string>

#include <lvgl/src/draw/lv_image_dsc.h>
#include <lvgl/src/misc/lv_color.h>

namespace LeleImageConverter {
enum TileRepeat {
    RepeatX,
    RepeatY,
    RepeatXY
};
void writeLvImgDscCpp(std::ofstream &c_img_filestream, const std::map<std::string, std::string> &img_file_hashes);
void writeLvImgDscHeader(const std::string &out_dir, const std::map<std::string, std::string> &img_file_hashes);
std::optional<std::pair<std::string, std::string>> generateCImgFile(std::ofstream &c_img_filestream, const std::string &img_file_path);
std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> generateImgDsc(const std::string &img_file_path);
std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> generateImgDsc(int width, int height, int bpp);
void fillImgDsc(lv_image_dsc_t *img_dsc, uint32_t color);
std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> resizeImg(const lv_image_dsc_t *src_img, int new_width, int new_height);
std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> tileImg(const lv_image_dsc_t *src_img, int new_width, int new_height,
    TileRepeat repeat = TileRepeat::RepeatXY, int dx = 0, int dy = 0);
std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> cropImg(const lv_image_dsc_t *src_img, int x, int y, int cropped_width, int cropped_height);
std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> rotateImg(const lv_image_dsc_t *src_img, int pivot_x, int pivot_y, float angle);
void saveGdImage(const std::string &filename, const lv_image_dsc_t *src_img);
}//namespace LeleImageConverter
