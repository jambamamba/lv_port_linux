#pragma once

#include <functional>
#include <stdint.h>

#include "jpeg_helper.h"
#include "png_helper.h"

class ImgHelper {

    public:
    ~ImgHelper();
    void processImgFile(const std::string &img_file_path, std::function<bool(uint8_t *row, size_t num_bytes)> scanline);
    int width() const;
    int height() const;
    size_t stride() const;

    protected:
    PngHelper _png;
    JpegHelper _jpeg;
    int _width;
    int _height;
    int _stride;
};