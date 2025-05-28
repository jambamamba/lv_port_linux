#pragma once

#include <libpng/png.h>
#include <cstdio>
#include <stdint.h>
#include <string>
#include <memory>
#include <functional>
// #include <pngstruct.h>
// #include <pnginfo.h>

class PngHelper {
    int _width;
    int _height;
    int _stride;
    int _bitdepth;
    int _channels;
    uint8_t**_row_pointers = NULL;

    public:
    ~PngHelper();
    void cleanup();
    bool readPngFile(
        const char *filename
    );
    void writePngFile(
        const char *filename, 
        int width, 
        int  height, 
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
    void processPngFile(std::function<bool(uint8_t *row, size_t num_bytes)> scanline);
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
};

