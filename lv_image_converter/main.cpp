

#include <filesystem>
#include <fstream>
#include <fstream>
#include <functional>
#include <iostream>
#include <iostream>
#include <string>
#include <map>
#include <openssl/sha.h>

#include "img_helper.h"

namespace {
bool iterateDirectory(const std::string &base_path, 
    std::function<bool(const std::string &)> entry_callback) {
    // Directory iteration using filesystem:
    for (const auto& entry : std::filesystem::directory_iterator(base_path)) {
        if(!entry_callback(entry.path())) {
            break;
        }
    }
    return true;
}

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

std::pair<std::string, std::string> generateCImgFile(std::ofstream &c_img_filestream, const std::string &img_file_path) {

    std::filesystem::path img_path(img_file_path);
    if(!std::filesystem::is_regular_file(img_file_path)) {
        return std::pair<std::string, std::string>("","");
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
    c_img_filestream << c_file_begin;

    ImgHelper img;
    img.processImgFile(img_file_path, [&c_img_filestream](const uint8_t *row, size_t num_bytes){

        const char index[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
        for(size_t col = 0; col < num_bytes; ++col) {
            uint8_t byte = row[col];
            c_img_filestream << "0x" << index[((byte & 0xf0) >> 4)] << index[((byte & 0x0f))] << ",";
        }
        return true;
    });
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
static const lv_img_dsc_t img_";
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

    c_img_filestream << "extern const std::map<std::string, const lv_img_dsc_t*> _lv_img_dsc_map;\n";
}
void writeLvImgDscCpp(std::ofstream &c_img_filestream, const std::map<std::string, std::string> &img_file_hashes) {

    c_img_filestream << "const std::map<std::string, const lv_img_dsc_t*> _lv_img_dsc_map = {\n";
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
}//namespace

int main(int argc, char **argv) {
    printf("hello \n");
    if(argc < 3) {
        printf("syntax: ./lv_image_converter /input/dir /output/dir #where /input/dir can contain png or jpg files\n");
        return -1;
    }
    std::filesystem::path path(argv[1]);
    std::filesystem::exists(path);
    std::filesystem::create_directory(argv[2]);

    std::string c_file_path(argv[2]);
    c_file_path += "/img_dsc.cpp";
    std::ofstream c_img_filestream(c_file_path);
    c_img_filestream << "\
#include <res/img_dsc.h>\n\
\n\
#ifndef LV_ATTRIBUTE_MEM_ALIGN\n\
#define LV_ATTRIBUTE_MEM_ALIGN\n\
#endif\n\
";

    std::map<std::string, std::string> img_file_hashes;
    if(std::filesystem::is_regular_file(path)) {
        img_file_hashes.insert(generateCImgFile(c_img_filestream, argv[1]));
    }
    else if(std::filesystem::is_directory(path)) {
        iterateDirectory(argv[1], [&c_img_filestream, &img_file_hashes](const std::string &filename){
            img_file_hashes.insert(generateCImgFile(c_img_filestream, filename));
            return true;
        });
    }
    else {
        printf("bad path:%s\n", argv[1]);
        return -1;
    }

    writeLvImgDscHeader(argv[2], img_file_hashes);
    writeLvImgDscCpp(c_img_filestream, img_file_hashes);

    return 0;
}