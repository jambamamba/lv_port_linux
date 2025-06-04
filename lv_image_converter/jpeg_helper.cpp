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

	// printf("@@@ w:%i,h:%i,s:%i,bpp:%i\n", _width, _height, _stride, _stride/_width);

    return true;
}

void JpegHelper::processJpegFile(std::function<bool(const uint8_t *row, size_t num_bytes)> scanline) {

    unsigned char *row = static_cast<unsigned char *>(malloc(_stride));
	unsigned char *rows[1];
	rows[0] = row;
    while (_cinfo.output_scanline < _cinfo.output_height) {
        if(1 == jpeg_read_scanlines(&_cinfo, rows, 1)) {
		    for(int i = 0; i < _stride; i+=_cinfo.output_components) {//_color_type:RGB
				uint8_t red = row[i+0];
				uint8_t green = row[i+1];
				uint8_t blue = row[i+2];
				row[i+0]=blue;
				row[i+1]=green;
				row[i+2]=red;
			}
            if(!scanline(row, _stride)){
                break;
            }
        }
    }
	// free(row);
	free(rows[0]);
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