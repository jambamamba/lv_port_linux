#pragma once

#include <functional>
#include <memory>
#include <stdint.h>
#include <stdio.h>
#include <jpegsr9e/jpeglib.h>

class JpegHelper {

    public:
    ~JpegHelper();
    bool readJpegFile(
        const char *filename
    );
    void processJpegFile(std::function<bool(const uint8_t *row, size_t num_bytes)> scanline);
    int width() const;
    int height() const;
    size_t stride() const;

    protected:
    struct jpeg_decompress_struct _cinfo = {0};
	struct jpeg_error_mgr _jerr;
	std::unique_ptr<char []>_jpg_buffer;
    int _width;
    int _height;
    int _stride;
};
