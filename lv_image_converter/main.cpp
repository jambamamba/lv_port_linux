

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

std::string sha256(const std::string &inputStr)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    const unsigned char* data = (const unsigned char*)inputStr.c_str();
    SHA256(data, inputStr.size(), hash);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)std::hash<int>{}(i);
    }
    return ss.str();
}

std::pair<std::string, std::string> generateCImgFile(const std::string &img_file_path, const std::string &out_dir) {

    printf("[%s:%i] processing %s\n", __FILE__, __LINE__, img_file_path.c_str());
    std::filesystem::path img_path(img_file_path);
    if(!std::filesystem::is_regular_file(img_file_path)) {
        return std::pair<std::string, std::string>("","");
    }
    std::string img_path_hash = sha256(img_path.stem());

    std::string c_file_begin("\n\
#ifdef __has_include\n\
    #if __has_include(\"lvgl.h\")\n\
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE\n\
            #define LV_LVGL_H_INCLUDE_SIMPLE\n\
        #endif\n\
    #endif\n\
#endif\n\
\n\
#include \"lvgl/lvgl.h\"\n\
\n\
#if LV_USE_DEMO_WIDGETS\n\
#ifndef LV_ATTRIBUTE_MEM_ALIGN\n\
#define LV_ATTRIBUTE_MEM_ALIGN\n\
#endif\n\
\n\
#ifndef LV_ATTRIBUTE_IMAGE_IMG_LVGL_LOGO\n\
#define LV_ATTRIBUTE_IMAGE_IMG_LVGL_LOGO\n\
#endif\n\
\n\
static const\n\
LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMAGE_IMG_LVGL_LOGO\n\
uint8_t img_");
        c_file_begin += img_path_hash;
        c_file_begin += "_map[] = {\n\
";

    std::filesystem::create_directory(out_dir.c_str());
    std::string c_file_path(out_dir.c_str());
    c_file_path += "/";
    c_file_path += img_path_hash;
    c_file_path += ".c";
    std::ofstream c_img_file(c_file_path);
    c_img_file << c_file_begin;

    ImgHelper img;
    img.processImgFile(img_file_path, [&c_img_file](const uint8_t *row, size_t num_bytes){

        const char index[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
        for(size_t col = 0; col < num_bytes; ++col) {
            uint8_t byte = row[col];
            c_img_file << "0x" << index[((byte & 0xf0) >> 4)] << index[((byte & 0x0f))] << ",";
        }
        return true;
    });

    std::string c_file_end("\n\
\n\
};\n\
\n\
const lv_img_dsc_t img_");
    c_file_end += img_path_hash;
    c_file_end += " = {\n\
  .header.magic = LV_IMAGE_HEADER_MAGIC,\n\
  .header.cf = LV_COLOR_FORMAT_ARGB8888,\n\
  .header.flags = 0,\n\
  .header.w = ";
    c_file_end += std::to_string(img.width());
    c_file_end += ",\n\
  .header.h = ";
    c_file_end += std::to_string(img.height());
    c_file_end += ",\n\
  .header.stride = ";
    c_file_end += std::to_string(img.stride());
    c_file_end +=",\n\
  .data_size = ";
    c_file_end += std::to_string(img.stride() * img.height());
    c_file_end +=",\n\
  .data = img_";
    c_file_end += img_path_hash;
    c_file_end +="_map,\n\
};\n\
\n\
#endif\n\
    ";

    c_img_file << c_file_end;
    printf("width:%i, height:%i\n", img.width(), img.height());
    return std::pair<std::string, std::string>(img_path.filename(), img_path_hash);
}

void writeLvImgDscHeader(const std::string &out_dir, const std::map<std::string, std::string> &img_file_hashes) {
    std::string c_file_path(out_dir.c_str());
    c_file_path += "/lv_img_dsc.h";
    std::ofstream c_img_file(c_file_path);
    c_img_file << "#pragma once\n";
    c_img_file << "\n";
    c_img_file << "#include <map>\n";
    c_img_file << "#include <string>\n";
    c_img_file << "#include <lvgl/lvgl.h>\n";
    c_img_file << "\n";

    for(const auto &[img_file_name, img_path_hash] : img_file_hashes) {
        c_img_file << "extern const lv_img_dsc_t img_" << img_path_hash << ";\n";
    }
    c_img_file << "\n";
    c_img_file << "extern const std::map<std::string, const lv_img_dsc_t*> _lv_img_dsc_map;\n";
}
void writeLvImgDscCpp(const std::string &out_dir, const std::map<std::string, std::string> &img_file_hashes) {
    std::string c_file_path(out_dir.c_str());
    c_file_path += "/lv_img_dsc.cpp";
    std::ofstream c_img_file(c_file_path);
    c_img_file << "#include <lvgl/lvgl.h>\n";
    c_img_file << "#include <res/lv_img_dsc.h>\n";
    c_img_file << "\n";
    c_img_file << "const std::map<std::string, const lv_img_dsc_t*> _lv_img_dsc_map = {\n";
    int idx = 0;
    for(const auto &[img_file_name, img_path_hash] : img_file_hashes) {
        if(idx > 0) {
            c_img_file << ",";
        }
        c_img_file << "{\"" << img_file_name << "\", &img_" << img_path_hash << "}\n";
        ++idx;
    }
    c_img_file << "};\n";
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
    std::map<std::string, std::string> img_file_hashes;
    if(std::filesystem::is_regular_file(path)) {
        img_file_hashes.insert(generateCImgFile(argv[1], argv[2]));
    }
    else if(std::filesystem::is_directory(path)) {
        iterateDirectory(argv[1], [argv, &img_file_hashes](const std::string &filename){
            img_file_hashes.insert(generateCImgFile(filename, argv[2]));
            return true;
        });
    }
    else {
        printf("bad path:%s\n", argv[1]);
        return -1;
    }

    writeLvImgDscHeader(argv[2], img_file_hashes);
    writeLvImgDscCpp(argv[2], img_file_hashes);

    return 0;
}