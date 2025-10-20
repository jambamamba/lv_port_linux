#include "lv_image_converter.h"

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
}//namespace

int main(int argc, char **argv) {
    if(argc < 3) {
        printf("syntax: ./lv_image_converter /input/dir /output/dir #where /input/dir can contain png or jpg files\n");
        return -1;
    }
    std::filesystem::path path(argv[1]);
    if(!std::filesystem::exists(path)) {
        std::filesystem::create_directory(argv[2]);
    }

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
        auto res = LeleImageConverter
::generateCImgFile(c_img_filestream, argv[1]);
        if(res) {
            img_file_hashes.insert(res.value());
        }
    }
    else if(std::filesystem::is_directory(path)) {
        iterateDirectory(argv[1], [&c_img_filestream, &img_file_hashes](const std::string &filename){
            auto res = LeleImageConverter
::generateCImgFile(c_img_filestream, filename);
            if(res) {
                img_file_hashes.insert(res.value());
            }
            return true;
        });
    }
    else {
        printf("bad path:%s\n", argv[1]);
        return -1;
    }

    // printf("@@@ img_file_hashes.size:%li\n", img_file_hashes.size());
    // for(const auto &[filename, filehash] : img_file_hashes) {
    //     printf("@@@ %s\n", filename.c_str());
    // }

    LeleImageConverter
::writeLvImgDscHeader(argv[2], img_file_hashes);
    LeleImageConverter
::writeLvImgDscCpp(c_img_filestream, img_file_hashes);

    return 0;
}