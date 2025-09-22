#pragma once

#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <optional>
#include <smart_pointer/auto_free_ptr.h>
#include <string>

#include <lvgl/src/draw/lv_image_dsc.h>
#include <lvgl/src/misc/lv_color.h>

void writeLvImgDscCpp(std::ofstream &c_img_filestream, const std::map<std::string, std::string> &img_file_hashes);
void writeLvImgDscHeader(const std::string &out_dir, const std::map<std::string, std::string> &img_file_hashes);
std::optional<std::pair<std::string, std::string>> generateCImgFile(std::ofstream &c_img_filestream, const std::string &img_file_path);
std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> generateImgDsc(const std::string &img_file_path);