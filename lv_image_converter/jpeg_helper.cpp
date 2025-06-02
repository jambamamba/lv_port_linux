#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>

#include "jpeg_helper.h"

JpegHelper::~JpegHelper() {
    // jpeg_finish_decompress(&_cinfo);
    jpeg_destroy_decompress(&_cinfo);
}

bool JpegHelper::readJpegFile(const char *filename) {

	struct stat file_info;
	int rc = stat(filename, &file_info);
	if (rc) {
		printf("No such file: %s\n", filename);
		return false;
	}
	_jpg_buffer = std::make_unique<char[]>(file_info.st_size);

	std::ifstream infile(filename, std::ios::binary);
	infile.read(_jpg_buffer.get(), file_info.st_size);

	_cinfo.err = jpeg_std_error(&_jerr);	
	jpeg_create_decompress(&_cinfo);
	jpeg_mem_src(&_cinfo, reinterpret_cast<unsigned char*>(_jpg_buffer.get()), file_info.st_size);
	rc = jpeg_read_header(&_cinfo, TRUE);
	if (rc != 1) {
		printf("Not a JPEG file\n");
		return false;
	}
	jpeg_start_decompress(&_cinfo);

    _width = _cinfo.output_width;
    _height = _cinfo.output_height;
    _stride = _cinfo.output_width * _cinfo.output_components;

    return true;
}

void JpegHelper::processJpegFile(std::function<bool(const uint8_t *row, size_t num_bytes)> scanline) {

	unsigned char *buffer_array[1];
    buffer_array[0] = static_cast<unsigned char *>(malloc(_stride));
    while (_cinfo.output_scanline < _cinfo.output_height) {
        if(1 == jpeg_read_scanlines(&_cinfo, buffer_array, 1)) {
            if(!scanline(buffer_array[0], _stride)){
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