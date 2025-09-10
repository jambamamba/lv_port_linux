#include "img_helper.h"

ImgHelper::~ImgHelper() {}

bool ImgHelper::processImgFile(const std::string &img_file_path, std::function<bool(const uint8_t *row, size_t num_bytes)> scanline) {
    if(_png.readPngFile(img_file_path.c_str())) {
        _width = _png.width();
        _height = _png.height();
        _stride = _png.stride();
        if(scanline) {
            _png.processPngFile([scanline](const uint8_t *row, size_t num_bytes){
                return scanline(row, num_bytes);
            });
        }
        return true;
    }
    // if(_heif.readHeifFile(img_file_path.c_str())) {
    //     _width = _heif.width();
    //     _height = _heif.height();
    //     _stride = _heif.stride();
    //     _heif.processHeifFile([scanline](const uint8_t *row, size_t num_bytes){
    //         return scanline(row, num_bytes);
    //     });
    //  return true;
    // }
    if(_jpeg.readJpegFile(img_file_path.c_str())) {
        _width = _jpeg.width();
        _height = _jpeg.height();
        _stride = _jpeg.stride();
    	// printf("@@@[%s:%i] w:%i,h:%i,s:%i,bpp:%i\n", __FILE__, __LINE__, _width, _height, _stride, _stride/_width);
        // exit(-1);
        if(scanline) {
            _jpeg.processJpegFile([scanline](const uint8_t *row, size_t num_bytes){
                return scanline(row, num_bytes);
                });
        }
        return true;
    }
    
    printf("'%s' Not a jpeg or png or heif, ignoring file!\n", img_file_path.c_str());
    return false;
}

int ImgHelper::width() const{
    return _width;
}
int ImgHelper::height() const{
    return _height;
}
size_t ImgHelper::stride() const{
    return _stride;
}