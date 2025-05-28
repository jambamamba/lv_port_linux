

#include "png_helper.h"

#include <filesystem>
#include <fstream>
#include <fstream>
#include <functional>
#include <iostream>
#include <iostream>
#include <string>

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

bool generateCImgFile(const std::string &img_file_path) {

    const std::string c_file_begin("\n\
    #ifdef __has_include\n\
    #if __has_include(\"lvgl.h\")\n\
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE\n\
            #define LV_LVGL_H_INCLUDE_SIMPLE\n\
        #endif\n\
    #endif\n\
#endif\n\
\n\
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)\n\
    #include \"lvgl.h\"\n\
#else\n\
    #include \"lvgl/lvgl.h\"\n\
#endif\n\
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
uint8_t img_lvgl_logo_map[] = {\n\
");

    printf("[%s:%i] processing %s\n", __FILE__, __LINE__, img_file_path.c_str());

    std::filesystem::path img_path(img_file_path);
    if(!std::filesystem::is_regular_file(img_file_path)) {
        return true;
    }

    std::filesystem::create_directory(".res");
    std::string c_file_path(".res/");
    c_file_path += img_path.stem();
    c_file_path += ".c";
    std::ofstream c_img_file(c_file_path);
    c_img_file << c_file_begin;

    PngHelper png;
    if(!png.readPngFile(img_file_path.c_str())) {
        return true;
    }
    png.processPngFile([&c_img_file](uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3){

        const char index[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
        c_img_file << 
            "0x" << index[((byte0 & 0xf0) >> 4)] << index[((byte0 & 0x0f))] << "," <<
            "0x" << index[((byte1 & 0xf0) >> 4)] << index[((byte1 & 0x0f))] << "," <<
            "0x" << index[((byte2 & 0xf0) >> 4)] << index[((byte2 & 0x0f))] << "," <<
            "0x" << index[((byte3 & 0xf0) >> 4)] << index[((byte3 & 0x0f))] << ",";
        return true;
    });

    std::string c_file_end("\n\
\n\
};\n\
\n\
const lv_img_dsc_t img_");
    c_file_end += img_path.stem();
    c_file_end += "{\n\
  .header.magic = LV_IMAGE_HEADER_MAGIC,\n\
  .header.cf = LV_COLOR_FORMAT_ARGB8888,\n\
  .header.flags = 0,\n\
  .header.w = ";
    c_file_end += std::to_string(png.width());
    c_file_end += ",\n\
  .header.h = ";
    c_file_end += std::to_string(png.height());
    c_file_end += ",\n\
  .header.stride = ";
    c_file_end += std::to_string(png.rowSize());
    c_file_end +=",\n\
  .data_size = ";
    c_file_end += std::to_string(png.rowSize() * png.height());
    c_file_end +=",\n\
  .data = img_";
    c_file_end += img_path.stem();
    c_file_end +=",\n\
};\n\
\n\
#endif\n\
    ";

    c_img_file << c_file_end;
    printf("width:%i, height:%i\n", png.width(), png.height());
    return true;
}

int main(int argc, char **argv) {
    printf("hello \n");
    if(argc < 2) {
        printf("syntax: ./lv_image_converter /path/to/img.png\n");
        return -1;
    }
    std::filesystem::path path(argv[1]);
    std::filesystem::exists(path);
    if(std::filesystem::is_regular_file(path)) {
        return generateCImgFile(argv[1]);
    }
    else if(std::filesystem::is_directory(path)) {
        iterateDirectory(argv[1], [](const std::string &filename){
            return generateCImgFile(filename);
        });
    }
    else {
        printf("bad path:%s\n", argv[1]);
        return -1;
    }
    return 0;
}