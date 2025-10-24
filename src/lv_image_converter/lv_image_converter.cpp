#include "lv_image_converter.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <iostream>
#include <openssl/sha.h>

#include <utils/img_helper.h>

namespace {
std::string sha256sum(const std::string &input_str) {
    unsigned char hash[SHA256_DIGEST_LENGTH] = {0};
    const unsigned char* data = (const unsigned char*)input_str.c_str();
    SHA256(data, input_str.size(), hash);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

void initImageDsc(lv_image_dsc_t *dst_img, int width, int height, int bpp) {
    uint32_t color_format;
    switch(bpp) {
        case 4: color_format = LV_COLOR_FORMAT_ARGB8888; break;
        case 3: color_format = LV_COLOR_FORMAT_RGB888; break;
        default: printf("invalid bpp, color format could not be determined from bpp:%i\n", bpp); exit(-1);
    }
    dst_img->header = lv_image_header_t {
        .magic = LV_IMAGE_HEADER_MAGIC,
        .cf = color_format,
        .flags = 0,
        .w = static_cast<uint32_t>(width),
        .h = static_cast<uint32_t>(height),
        .stride = static_cast<uint32_t>(width * bpp)
      };
    dst_img->data_size = width * bpp * height;
    dst_img->data = reinterpret_cast<uint8_t*>(&dst_img[1]);
}

}//namespace

namespace LeleImageConverter {

std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> resizeImg(lv_image_dsc_t *src_img, int new_width, int new_height) {
    int bpp = src_img->header.stride/src_img->header.w;
    auto dst_img = AutoFreeSharedPtr<lv_image_dsc_t>::create(new_width * bpp * new_height);
    initImageDsc(dst_img.get(), new_width, new_height, bpp);

    ImgHelper img;
    if(!img.resizeImageData(src_img->header.w, src_img->header.h, src_img->header.stride, src_img->data,
        new_width, new_height, const_cast<uint8_t*>(dst_img->data))) {
        return std::nullopt;
    }
    return dst_img;
}

std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> tileImg(lv_image_dsc_t *src_img, int new_width, int new_height) {
    int bpp = src_img->header.stride/src_img->header.w;
    auto dst_img = AutoFreeSharedPtr<lv_image_dsc_t>::create(new_width * bpp * new_height);
    initImageDsc(dst_img.get(), new_width, new_height, bpp);

    ImgHelper img;
    if(!img.tileImageData(src_img->header.w, src_img->header.h, src_img->header.stride, src_img->data,
        new_width, new_height, const_cast<uint8_t*>(dst_img->data))) {
        return std::nullopt;
    }
    return dst_img;
}

std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> generateImgDsc(const std::string &img_file_path) {

    std::filesystem::path img_path(img_file_path);
    if(!std::filesystem::is_regular_file(img_file_path)) {
        // return AutoFreePtr<lv_image_dsc_t>::nullopt();
        return std::nullopt;
    }

    ImgHelper img;
    if(!img.processImgFile(img_file_path)){
        // return AutoFreePtr<lv_image_dsc_t>::nullopt();
        return std::nullopt;
    }
    int bpp = img.stride()/img.width();
    auto img_dsc = AutoFreeSharedPtr<lv_image_dsc_t>::create(img.stride() * img.height());
    initImageDsc(img_dsc.get(), img.width(), img.height(), bpp);
    
    size_t bytes_copied = 0;
    lv_image_dsc_t *p = img_dsc.get();
    uint8_t *img_data = (uint8_t *)&p[1];
    p->data = img_data;
    if(!img.processImgFile(img_file_path, [img_data,&bytes_copied](const uint8_t *row, size_t num_bytes) {
        memcpy(&img_data[bytes_copied], (const void*)row, num_bytes);
        bytes_copied += num_bytes;
        return true;
    })) {
        // return AutoFreePtr<lv_image_dsc_t>::nullopt();
        return std::nullopt;
    }

    // printf("width:%i, height:%i\n", img.width(), img.height());
    return img_dsc;
}

std::optional<std::pair<std::string, std::string>> generateCImgFile(std::ofstream &c_img_filestream, const std::string &img_file_path) {

    std::filesystem::path img_path(img_file_path);
    if(!std::filesystem::is_regular_file(img_file_path)) {
        return std::nullopt;
    }
    std::string img_path_hash = sha256sum(img_path.filename());
    printf("[%s:%i] processing %s, hash:%s\n", __FILE__, __LINE__, img_file_path.c_str(), img_path_hash.c_str());

    std::string c_file_begin("\n//");
    c_file_begin += img_path.filename();
    c_file_begin += "\n\
static const\n\
LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST\n\
uint8_t img_";
        c_file_begin += img_path_hash;
        c_file_begin += "_map[] = {\n\
";

    std::string c_file_scanlines;
    ImgHelper img;
    if(!img.processImgFile(img_file_path, [&c_file_scanlines](const uint8_t *row, size_t num_bytes){

        const std::string index[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f"};
        for(size_t col = 0; col < num_bytes; ++col) {
            uint8_t byte = row[col];
            c_file_scanlines += "0x";
            c_file_scanlines += index[((byte & 0xf0) >> 4)];
            c_file_scanlines += index[((byte & 0x0f))];
            c_file_scanlines += ",";
        }
        return true;
    })) {
        return std::nullopt;
    }
    int bpp = img.stride()/img.width();
    std::string color_format;
    switch(bpp) {
        case 4: color_format = "LV_COLOR_FORMAT_ARGB8888"; break;
        case 3: color_format = "LV_COLOR_FORMAT_RGB888"; break;
        default: printf("invalid bpp, color format could not be determined from bpp:%i\n", bpp); exit(-1);
    }
    // printf("@@@[%s:%i] w:%i,h:%i,s:%i,bpp:%i,cf:%s\n", __FILE__, __LINE__, img.width(), img.height(), img.stride(), bpp, color_format.c_str());
    // exit(-1);

    std::string c_file_end("\n\
};\n\
\n\
//");
    c_file_end += img_path.filename();
    c_file_end += "\n\
static const lv_image_dsc_t img_";
    c_file_end += img_path_hash;
    c_file_end += " = {\n\
  .header = { \n\
    .magic = LV_IMAGE_HEADER_MAGIC,\n\
    .cf = ";
    c_file_end += color_format;
    c_file_end += ",\n\
    .flags = 0,\n\
    .w = ";
    c_file_end += std::to_string(img.width());
    c_file_end += ",\n\
    .h = ";
    c_file_end += std::to_string(img.height());
    c_file_end += ",\n\
    .stride = ";
    c_file_end += std::to_string(img.stride());
    c_file_end +="\n    },\n\
  .data_size = ";
    c_file_end += std::to_string(img.stride() * img.height());
    c_file_end +=",\n\
  .data = img_";
    c_file_end += img_path_hash;
    c_file_end +="_map,\n\
};\n\
\n\
";
    c_img_filestream << c_file_begin;
    c_img_filestream << c_file_scanlines;
    c_img_filestream << c_file_end;
    printf("width:%i, height:%i\n", img.width(), img.height());
    return std::pair<std::string, std::string>(img_path.filename(), img_path_hash);
}

void writeLvImgDscHeader(const std::string &out_dir, const std::map<std::string, std::string> &img_file_hashes) {
    std::string c_file_path(out_dir.c_str());
    c_file_path += "/img_dsc.h";
    std::ofstream c_img_filestream(c_file_path);
    c_img_filestream << "#pragma once\n\
\n\
#include <map>\n\
#include <string>\n\
#include <lvgl/lvgl.h>\n\
\n";

    c_img_filestream << "extern const std::map<std::string, const lv_image_dsc_t*> _lv_img_dsc_map;\n";
}
void writeLvImgDscCpp(std::ofstream &c_img_filestream, const std::map<std::string, std::string> &img_file_hashes) {

    c_img_filestream << "const std::map<std::string, const lv_image_dsc_t*> _lv_img_dsc_map = {\n";
    int idx = 0;
    for(const auto &[img_file_name, img_path_hash] : img_file_hashes) {
        if(img_file_name.empty()) {
            continue;
        }
        if(idx == 0) {
            c_img_filestream << " ";
        } 
        else {
            c_img_filestream << ",";
        }
        c_img_filestream << "{\"" << img_file_name << "\", &img_" << img_path_hash << "}\n";
        ++idx;
    }
    c_img_filestream << "};\n";
}
}//namespace LeleImageConverter
