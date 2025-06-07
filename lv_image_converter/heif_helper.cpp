#include "heif_helper.h"

namespace {
void resetCtx(heif_context *ctx) {
    if(ctx) {
        heif_context_free(ctx);
    }
}
}//namespace

HeifHelper::~HeifHelper(){
    resetCtx(_ctx);
}
bool HeifHelper::readHeifFile(const char *filename){
    resetCtx(_ctx);
    _ctx = heif_context_alloc();
    struct heif_error ret = heif_context_read_from_file(_ctx, filename, nullptr);
    return ret.code == heif_error_Ok;
}

void HeifHelper::processHeifFile(std::function<bool(const uint8_t *row, size_t num_bytes)> scanline){
    if(!_ctx) {
        return;
    }
    // get a handle to the primary image
    heif_image_handle* handle = nullptr;
    heif_context_get_primary_image_handle(_ctx, &handle);

    // decode the image and convert colorspace to RGB, saved as 24bit interleaved
    heif_image* img = nullptr;
    heif_decode_image(handle, &img, heif_colorspace_RGB, heif_chroma_interleaved_RGB, nullptr);

    _width = 0;
    _height = 0;
    _stride = 0;
    const uint8_t* data = heif_image_get_plane_readonly(img, heif_channel_interleaved, &_stride);
    if(data && _stride > 0) {
        _width = heif_image_handle_get_width(handle);
        _height = heif_image_handle_get_height(handle);
    
        for(int row = 0; row < _height; ++row) {
            if(!scanline(&data[row * _stride], _stride)) {
                break;
            }
        }
    }

    heif_image_release(img);
    heif_image_handle_release(handle);
}

int HeifHelper::width() const{
    return _width;
}
int HeifHelper::height() const{
    return _height;
}
size_t HeifHelper::stride() const{
    return _height;
}

