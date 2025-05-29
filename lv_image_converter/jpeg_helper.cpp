#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#include "jpeg_helper.h"

struct my_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void) my_error_exit (j_common_ptr _cinfo) {
    my_error_ptr myerr = (my_error_ptr) _cinfo->err;
    (*_cinfo->err->output_message) (_cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

JpegHelper::~JpegHelper() {
    jpeg_destroy_decompress(&_cinfo);
}

bool JpegHelper::readJpegFile(const char *filename) {
    struct my_error_mgr jerr;
    FILE *infile = nullptr;

    _cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&_cinfo);
        if(infile) fclose(infile);
        return false;
    }

    jpeg_create_decompress(&_cinfo);

    if ((infile = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "Can't open %s\n", filename);
        return false;
    }
    jpeg_stdio_src(&_cinfo, infile);

    jpeg_read_header(&_cinfo, TRUE);

    jpeg_start_decompress(&_cinfo);

    _width = _cinfo.output_width;
    _height = _cinfo.output_height;
    _stride = _cinfo.output_width * _cinfo.output_components;

    printf("@@@ w:%i, h:%i, s:%i\n", _width, _height, _stride);

    fclose(infile);

    return true;
}

void JpegHelper::processJpegFile(std::function<bool(const uint8_t *row, size_t num_bytes)> scanline) {

    JSAMPARRAY buffer = (*_cinfo.mem->alloc_sarray) ((j_common_ptr) &_cinfo, JPOOL_IMAGE, _stride, 1);

    while (_cinfo.output_scanline < _cinfo.output_height) {
        if(1 == jpeg_read_scanlines(&_cinfo, buffer, 1)) {
            if(!scanline(buffer[0], _stride)){
                break;
            }
        }
    }
}

int JpegHelper::width() const{
    return _width;
}
int JpegHelper::height() const{
    return _height;
}
size_t JpegHelper::stride() const{
    return _stride;
}