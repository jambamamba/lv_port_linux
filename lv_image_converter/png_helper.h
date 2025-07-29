#pragma once

#include <libpng/png.h>
#include <cstdio>
#include <stdint.h>
#include <string>
#include <memory>
#include <functional>
#include "auto_free_ptr.h"
// #include <pngstruct.h>
// #include <pnginfo.h>

struct PngContext;
class PngHelper {
public:
    struct MetaData {
        int _width = 0;
        int _height = 0;
        int _stride = 0;//bytes per row
        int _bit_depth = 0;
        int _channels = 0;
        png_byte _color_type = 0;
    };
    ~PngHelper();
    void cleanup();
    bool readPngFile(
        const char *filename
    );
    // AutoFreePtr<PngContext> readPngFile(
    //     const char *filename
    // );
    void writePngFile(
        const char *filename, 
        int width, 
        int height, 
        int bitdepth,
        int bits_per_pixel,
        png_bytep data, 
        int data_bitdepth,
        bool strip_alpha,
        int horizontal_padding,
        int right_crop);
    void padToSize(
        int horizontal_padding,
        const std::string &tmpname = std::tmpnam(nullptr));
    void processPngFile(std::function<bool(const uint8_t *row, size_t num_bytes)> scanline);
    int width() const;
    int height() const;
    size_t stride() const;
    int bitdepth() const;
    int channels() const;
    png_bytep rowPointer(int row) const;
    png_byte data(int idx) const;
    std::unique_ptr<uint8_t[]> data() const;
    bool operator==(const PngHelper &rhs) const;
    png_byte operator[](int idx) const;
    PngHelper convertTo64bpp(const std::string &filename = std::tmpnam(nullptr));
protected:
    PngHelper::MetaData _mtd;
    // std::unique_ptr<std::unique_ptr<png_byte[]>[]> _row_pointers;
    png_byte **_row_pointers = nullptr;
};

struct PngContext {
    PngHelper::MetaData _mtd;
    size_t _size;
    unsigned char _data[1];

    void init(const PngHelper::MetaData &mtd, size_t nbytes){
        _mtd = mtd;
        _size = nbytes;
    }
};