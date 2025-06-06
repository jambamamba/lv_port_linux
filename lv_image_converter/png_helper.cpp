#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <filesystem>
#include <mutex>
#include <stdarg.h>
#include <time.h>

#include "png_helper.h"
// #include "dlog.h"

namespace {
void myabort() {
    abort();
    return;
}

static long currentTimeMillis()  {

    struct timespec ts;

    if (timespec_get(&ts, TIME_UTC) != TIME_UTC) {
        return 0;
    }
    return 1000000000 * ts.tv_sec + ts.tv_nsec;
}

static std::string datetime() {
    time_t rawtime;
    struct tm *info;
    char time_stamp[80] = {0};
    time( &rawtime );
    info = localtime( &rawtime );
    strftime(time_stamp, sizeof(time_stamp),"%x-%I:%M:%S%p", info);
    int sz = snprintf(NULL, 0, "%s.%li", time_stamp, currentTimeMillis());
    if(sz >= sizeof(time_stamp)){
        fprintf(stdout, "time_stamp line is too long at %i bytes, max line length limit is %i\n", sz, (int)sizeof(time_stamp)-1);
        myabort();
    }
    snprintf(time_stamp + strlen(time_stamp), sizeof(time_stamp) - strlen(time_stamp) - 1, ".%li", currentTimeMillis());
	return time_stamp;
}

void dlog(const char* filename, int linenum, const char *format, ...){

	static std::mutex mut;

	std::lock_guard lock(mut);
	static char buffer[1024 * 1024] = {0};
	va_list args;
	va_start (args, format);
    snprintf(buffer, sizeof(buffer), " [%s:%i] ", filename, linenum);
    size_t pos = strlen(buffer);
  	vsnprintf (buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), format, args);
    if(buffer[pos] == '\r'){
        buffer[0] = '\r';
        buffer[pos] = ' ';
    }
  	va_end (args);

	static bool first = true;

	FILE *fp = fopen("/tmp/log.txt", "a+t");
	if(first) {
		fprintf(fp, "\nSTART@ %s =============================================================================================\n", datetime().c_str());
		first = false;
	}
	fwrite(buffer, strlen(buffer), 1, fp);
	fclose(fp);

	fwrite(buffer, strlen(buffer), 1, stdout);
}
}//namespace

PngHelper::~PngHelper() {
  cleanup();
}

void PngHelper::cleanup() {
  if(!_row_pointers) {
    return;
  }
  for(int y = 0; y < _height; y++) {
    if(_row_pointers[y]) {
      free(_row_pointers[y]);
    }
  }
  free(_row_pointers);
  _row_pointers = nullptr;
}

bool PngHelper::readPngFile(
  const char *filename) {

  FILE *fp = fopen(filename, "rb");
  if(!fp) {
    dlog(__FILE__, __LINE__, "FATAL ERROR: Failed to read file: %s\n", filename);
    return false;
  }

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png) {
    return false;
  }

  png_infop info = png_create_info_struct(png);
  if(!info) {
    return false;
  }

  if(setjmp(png_jmpbuf(png))) {
    return false;
  }

  png_init_io(png, fp);

  png_read_info(png, info);

  _width      = png_get_image_width(png, info);
  _height     = png_get_image_height(png, info);
  _bitdepth   = png_get_bit_depth(png, info);
  _color_type = png_get_color_type(png, info);

  // Read any color_type into 8bit depth, RGBA format.
  // See http://www.libpng.org/pub/png/libpng-manual.txt

  // if(_bitdepth == 16) {
  //   png_set_strip_16(png);
  //   }

  if(_color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(png);
  }

  // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
  if(_color_type == PNG_COLOR_TYPE_GRAY && _bitdepth < 8) {
    png_set_expand_gray_1_2_4_to_8(png);
  }

  if(png_get_valid(png, info, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png);
  }

  // These color_type don't have an alpha channel then fill it with 0xff.
  if(_color_type == PNG_COLOR_TYPE_RGB ||
     _color_type == PNG_COLOR_TYPE_GRAY ||
     _color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
  }

  if(_color_type == PNG_COLOR_TYPE_GRAY ||
     _color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_gray_to_rgb(png);
  }

  png_read_update_info(png, info);
  _channels   = png_get_channels(png, info);

  if (_row_pointers) { myabort(); }

  _stride = png_get_rowbytes(png,info);
  _row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * _height);
  for(int y = 0; y < _height; y++) {
    _row_pointers[y] = (png_byte*)malloc(_stride);
  }

  png_read_image(png, _row_pointers);

  fclose(fp);

  png_destroy_read_struct(&png, &info, NULL);
  return true;
}

void PngHelper::writePngFile(
  const char *filename, 
  int width, 
  int  height, 
  int img_bitdepth,
  int channels, 
  png_bytep data, 
  int data_bitdepth,
  bool strip_alpha,
  int horizontal_padding,
  int right_crop) {

  _width = width + horizontal_padding;
  _height = height;
  _bitdepth = img_bitdepth;
  _channels = channels;
  size_t num_samples = width * channels;
  _stride = num_samples * _bitdepth/8;

  int y;

  FILE *fp = fopen(filename, "wb");
  if(!fp) myabort();

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) myabort();

  png_infop info = png_create_info_struct(png);
  if (!info) myabort();

  if (setjmp(png_jmpbuf(png))) myabort();

  png_init_io(png, fp);

  // Output is 8bit depth, RGBA format.
  png_set_IHDR(
    png,
    info,
    _width - right_crop, 
    _height,
    _bitdepth,
    PNG_COLOR_TYPE_RGBA,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );
  png_set_compression_level(png, 0);
  png_write_info(png, info);

  // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
  if(strip_alpha) {
    png_set_filler(png, 0, PNG_FILLER_AFTER);
  }

  _row_pointers = (uint8_t**)malloc(sizeof(void*) * _height);
  for(int y = 0; y < _height; y++) {
    _row_pointers[y] = (uint8_t*)malloc((width + horizontal_padding) * channels * _bitdepth/8);
    switch(_bitdepth) {
      case 16: {
        uint16_t *data16 = (uint16_t *)data;
        for( size_t x = 0; x < num_samples; x+=4) {
          
          uint16_t *pixel = (uint16_t *)(_row_pointers[y]);
          uint16_t mask = (1 << (data_bitdepth - 8)) - 1;
          pixel[x + 0] = (data16[(y * num_samples) + x + 0] >> (data_bitdepth - 8)) | ((data16[(y * num_samples) + x + 0] & mask) << 8);
          pixel[x + 1] = (data16[(y * num_samples) + x + 1] >> (data_bitdepth - 8)) | ((data16[(y * num_samples) + x + 1] & mask) << 8);
          pixel[x + 2] = (data16[(y * num_samples) + x + 2] >> (data_bitdepth - 8)) | ((data16[(y * num_samples) + x + 2] & mask) << 8);
          pixel[x + 3] = (data16[(y * num_samples) + x + 3] >> (data_bitdepth - 8)) | ((data16[(y * num_samples) + x + 3] & mask) << 8);
          int debug = 0;
          debug = 1;
        }
        break;
      }
      case 8: {
        memcpy(_row_pointers[y], &data[y * num_samples], num_samples);
        break;
      }
      default: {
        dlog(__FILE__, __LINE__, "Unsupported bit depth of %i\n, only 8 and 16 are allowed\n", _bitdepth);
        return;
      }
    }
  }

  png_write_image(png, (png_bytep*)_row_pointers);
  png_write_end(png, NULL);

  fclose(fp);

  png_destroy_write_struct(&png, &info);

  for(int y = 0; y < _height; y++) {
    free(_row_pointers[y]);
  }
  free(_row_pointers);
  _row_pointers = nullptr;
}

void PngHelper::padToSize(
    int horizontal_padding,
    const std::string &tmpname) {

  png_bytep data = (png_bytep)malloc(_stride * _height);
  for(int y = 0; y < _height; y++) {
    png_bytep row = _row_pointers[y];
    memcpy((png_byte*)&data[y * _stride], _row_pointers[y], _stride);
  }

  PngHelper png;
	
  png.writePngFile(
    tmpname.c_str(), 
    _width, 
    _height, 
    _bitdepth,
    _channels, 
    data, 
    8,
    false,
    horizontal_padding,
    0);

  free(data);

  cleanup();
  readPngFile(tmpname.c_str());
}

PngHelper PngHelper::convertTo64bpp(const std::string &filename) {

  PngHelper png2;
  int row_bytes = width() * channels();
  uint16_t *data = (uint16_t *) calloc(1, row_bytes * sizeof(uint16_t) * height());
  for(size_t y = 0; y < height(); ++y) {
    for(size_t x = 0; x < row_bytes; x += 4) {
      data[(y * row_bytes) + x + 0] = (rowPointer(y)[x + 0] << 8);
      data[(y * row_bytes) + x + 1] = (rowPointer(y)[x + 1] << 8);
      data[(y * row_bytes) + x + 2] = (rowPointer(y)[x + 2] << 8);
      data[(y * row_bytes) + x + 3] = (rowPointer(y)[x + 3] << 8);
    }
  }
  png2.writePngFile(filename.c_str(), width(), height(), 16, channels(), (png_bytep)data, 16, false, 0, 0);
  free(data);

  PngHelper png3;
  png3.readPngFile(filename.c_str());
  return png3;
}

void PngHelper::processPngFile(std::function<bool(const uint8_t *row, size_t num_bytes)> scanline) {
  for(int y = 0; y < _height; y++) {
    png_bytep row = _row_pointers[y];
    for(int i = 0; i < _stride; i+=4) {//_color_type:PNG_COLOR_TYPE_RGBA
        uint8_t red = row[i+0];
        uint8_t green = row[i+1];
        uint8_t blue = row[i+2];
        uint8_t alpha = row[i+3];
        // if(i+3 == _stride-1) {
        // printf("@@@ %x,%x,%x,%x\n", row[i], row[i+1], row[i+2], row[i+3]);
        // }
        row[i+0] = blue;
        row[i+1] = green;
        row[i+2] = red;
        row[i+3] = alpha;
    }
    if(!scanline(row, _stride)){
      break;
    }
  }
}

int PngHelper::width() const {
  return _width;
}

int PngHelper::height() const {
  return _height;
}

size_t PngHelper::stride() const {
  return _stride;
}

int PngHelper::bitdepth() const {
  return _bitdepth;
}
int PngHelper::channels() const {
  return _channels;
}

png_bytep PngHelper::rowPointer(int row) const {
  if(row > -1 && row < _height) {
    return _row_pointers[row];
  }
  return nullptr;
}

png_byte PngHelper::data(int idx) const {
  int num_samples_per_row = _stride / _bitdepth * 8;
  int row = idx / num_samples_per_row;
  int col = idx - (row * num_samples_per_row);
  if(row > -1 && row < _height) {
    if(_bitdepth == 16) {
      return ((uint16_t*)_row_pointers[row])[col];
    }
    return _row_pointers[row][col];
  }
  return 0;
}

std::unique_ptr<uint8_t[]> PngHelper::data() const { //return pointer must be freed

	size_t samples_per_row = width() * channels();
  auto data_ = std::make_unique<uint8_t[]> (samples_per_row * bitdepth()/8 * height());
	for(size_t y = 0; y < height(); ++y) {
		for(size_t x = 0; x < samples_per_row; x += 4) {
      if(bitdepth() == 16) {
        uint16_t *data = (uint16_t*)data_.get();
        data[(y * samples_per_row) + x + 0] = ((uint16_t *)(rowPointer(y)))[x + 0];
        data[(y * samples_per_row) + x + 1] = ((uint16_t *)(rowPointer(y)))[x + 1];
        data[(y * samples_per_row) + x + 2] = ((uint16_t *)(rowPointer(y)))[x + 2];
        data[(y * samples_per_row) + x + 3] = ((uint16_t *)(rowPointer(y)))[x + 3];

        data[y * samples_per_row + x + 0] = (data[y * samples_per_row + x + 0] >> 8) | (data[y * samples_per_row + x + 0] << 8);
        data[y * samples_per_row + x + 1] = (data[y * samples_per_row + x + 1] >> 8) | (data[y * samples_per_row + x + 1] << 8);
        data[y * samples_per_row + x + 2] = (data[y * samples_per_row + x + 2] >> 8) | (data[y * samples_per_row + x + 2] << 8);
        data[y * samples_per_row + x + 3] = (data[y * samples_per_row + x + 3] >> 8) | (data[y * samples_per_row + x + 3] << 8);

      }
      else if(bitdepth() == 8) {
        data_[(y * samples_per_row) + x + 0] = (rowPointer(y)[x + 0]);
        data_[(y * samples_per_row) + x + 1] = (rowPointer(y)[x + 1]);
        data_[(y * samples_per_row) + x + 2] = (rowPointer(y)[x + 2]);
        data_[(y * samples_per_row) + x + 3] = (rowPointer(y)[x + 3]);
      }
      else {
        myabort();
      }
		}
	}
  return data_;
}

png_byte PngHelper::operator[](int idx) const {
  return data(idx);
}

bool PngHelper::operator==(const PngHelper &rhs) const {

  if(_width != rhs._width) {
    dlog(__FILE__, __LINE__, "width does not match: %i!=%i\n", _width, rhs._width);
    return false;
  }
  else if(_height != rhs._height) {
    dlog(__FILE__, __LINE__, "height does not match: %i!=%i\n", _height, rhs._height);
    return false;
  }
  else if(_stride != rhs._stride) {
    dlog(__FILE__, __LINE__, "stride does not match: %i!=%i\n", _stride, rhs._stride);
    return false;
  }

	for(int y = 0; y < _height; y++) {
    if(memcmp(_row_pointers[y], rhs._row_pointers[y], _stride) != 0 ) {
      dlog(__FILE__, __LINE__, "bytes at row %i do not match\n", y);
      for(size_t x = 0; x < _stride; ++x ){
        if( _row_pointers[y][x] != rhs._row_pointers[y][x]) {
          dlog(__FILE__, __LINE__, "offset %llu, 0x%02x!=0x%02x ", x, _row_pointers[y][x], rhs._row_pointers[y][x]);
          break;
        }
      }
      dlog(__FILE__, __LINE__, "\n");
      return false;
    }
	}

  dlog(__FILE__, __LINE__, "matches 100%%\n");
  return true;
}