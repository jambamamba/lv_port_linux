#pragma once

#include <functional>
#include <stdint.h>
#include <stdio.h>
#include <libheif/heif.h>

class HeifHelper {

    public:
    ~HeifHelper();
    bool readHeifFile(
        const char *filename
    );
    void processHeifFile(std::function<bool(const uint8_t *row, size_t num_bytes)> scanline);
    int width() const;
    int height() const;
    size_t stride() const;

    protected:
    heif_context *_ctx = nullptr;
    int _width;
    int _height;
    int _stride;
};
