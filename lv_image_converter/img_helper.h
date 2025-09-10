#pragma once

#include <functional>
#include <stdint.h>

#include "jpeg_helper.h"
#include "png_helper.h"
#include "heif_helper.h"

class ImgHelper {

    public:
    ~ImgHelper();
    bool processImgFile(const std::string &img_file_path, std::function<bool(const uint8_t *row, size_t num_bytes)> scanline = nullptr);
    int width() const;
    int height() const;
    size_t stride() const;

    protected:
    PngHelper _png;
    JpegHelper _jpeg;
    HeifHelper _heif;
    int _width = 0;
    int _height = 0;
    int _stride = 0;
};