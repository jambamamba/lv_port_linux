#include "lelecolorwheel.h"

#include "src/graphics_backend.h"
#include <lvgl/src/widgets/canvas/lv_canvas.h>
#include <math.h>

LOG_CATEGORY(LVSIM, "LVSIM");

namespace {

static constexpr float PI = 3.1415926535897932384626433832795f;

// Structure to hold RGB color values (0-1 range for this example)
typedef struct RGBColor {
    int r;
    int g;
    int b;
} RGBColor;

// Function to convert HSV to RGB
RGBColor HsvToRgb(float H, float S, float V) {
    float r = 0, g = 0, b = 0;
    // Normalize H to 0-360, S and V to 0-1 for internal calculation
    float h = H >= 360 ? 0 : H; // Wrap hue
    float s = S / 100.0f;
    float v = V / 100.0f;

    if (s == 0) {
        r = g = b = v; // Gray
    } else {
        int i = static_cast<int>(std::floor(h * 6.0f / 360.0f)); // Sector 0 to 5
        float f = h * 6.0f / 360.0f - i; // Fractional part of h
        float p = v * (1.0f - s);
        float q = v * (1.0f - s * f);
        float t = v * (1.0f - s * (1.0f - f));

        switch (i % 6) {
            case 0: r = v, g = t, b = p; break;
            case 1: r = q, g = v, b = p; break;
            case 2: r = p, g = v, b = t; break;
            case 3: r = p, g = q, b = v; break;
            case 4: r = t, g = p, b = v; break;
            case 5: r = v, g = p, b = q; break;
        }
    }

    RGBColor color;
    // Scale RGB values to 0-255 and round
    color.r = static_cast<int>(r * 255.0f);
    color.g = static_cast<int>(g * 255.0f);
    color.b = static_cast<int>(b * 255.0f);

    return color;
}


inline bool isWithinCircle(int width, int height, int x, int y) {

  int center_x = width / 2;
  int center_y = height / 2;
  float max_radius = std::min(center_x, center_y) * 0.9f;

  // Calculate the distance from the pixel to the center (using Pythagorean theorem)
  // dx = delta x, dy = delta y
  int dx = x - center_x;
  int dy = y - center_y;
  float distance = pow(dx * dx + dy * dy, .5f);

  return (distance <= max_radius);
}

void makeColorWheel(lv_obj_t* lv_obj, int width, int height, int bgcolor) {

    // Define the center coordinates of the canvas
    int center_x = width / 2;
    int center_y = height / 2;

    // Define the maximum radius for the color wheel
    float max_radius = std::min(center_x, center_y) * 0.9f; // Use 90% of the smallest half for padding

    // Loop through all pixels in the canvas
    for(int y = 0; y < height; ++y){
      for(int x = 0; x < width; ++x){

        // Calculate the distance from the pixel to the center (using Pythagorean theorem)
        // dx = delta x, dy = delta y
        int dx = x - center_x;
        int dy = y - center_y;
        float distance = pow(dx * dx + dy * dy, .5f);

        // Check if the pixel is within the main circular area
        // if (distance <= max_radius) {
        if(isWithinCircle(width, height, x, y)) {

            // Calculate the angle (hue) using the arctangent function (atan2)
            // The result is typically in the range [-PI, PI]
            float angle_radians = atan2(dy, dx);

            // Convert the angle from radians to degrees and adjust to a 0-360 range for HSL/HSV
            int angle_degrees = (angle_radians * 180. / PI); // Convert to degrees (-180 to 180)
            float hue = (angle_degrees + 360) % 360;      // Map to 0-360 range

            // Determine saturation: 100% at the edge, 0% at the center
            // The distance is mapped from [0, max_radius] to [0, 100]
            float saturation = (distance / max_radius) * 100.;

            // Set brightness to maximum for a full-intensity wheel
            float brightness = 100.;

            // Convert the calculated HSL/HSV values to an RGB color
            // This requires a separate conversion function
            RGBColor rgb = HsvToRgb(hue, saturation, brightness);

            // Set the pixel's color on the canvas
            // SET Pixel(x, y) TO rgb_color
            lv_color_t c = lv_color_make(rgb.r, rgb.g, rgb.b);
            lv_canvas_set_px(lv_obj, x, y, c, 0);
        }
        else {
            // If outside the wheel, set a background color (e.g., white)
            // SET Pixel(x, y) TO White // Or transparent, depending on implementation
            lv_color_t c = lv_color_make(
              (bgcolor >> 16) & 0xff, 
              (bgcolor >> 8) & 0xff, 
              (bgcolor >> 0) & 0xff);
            lv_canvas_set_px(lv_obj, x, y, c, 0);
        }
      }
    }
}

std::pair<int,int> getWidgetWidthHeight( 
  std::optional<LeleStyle::StyleValue> &&width_style, 
  std::optional<LeleStyle::StyleValue> &&height_style) {

  int width = 0;
  int height = 0;
  if(width_style && std::holds_alternative<int>(width_style.value())) {
    width = std::get<int>(width_style.value());
  }
  if(height_style && std::holds_alternative<int>(height_style.value())) {
    height = std::get<int>(height_style.value());
  }
  return std::pair<int,int>(width, height);
}

}//namespace

LeleColorWheel::LeleColorWheel(const std::string &json_str)
  : LeleObject(json_str) {
  _class_name = __func__;
  fromJson(json_str);
}

bool LeleColorWheel::fromJson(const std::string &json_str) {
  for (const auto &[key, token]: _nodes) {
    if (std::holds_alternative<std::string>(token)) {
      const std::string &value = std::get<std::string>(token);
      // if(key == "text") {
      //   _text = value;
      // }
    }
  }
  return true;
}

std::pair<int,int> LeleColorWheel::initCanvas() {
  auto [width, height] = getWidgetWidthHeight(getStyle("width"), getStyle("height"));
  if(width > 0 && height > 0) {
    constexpr lv_color_format_t cf = LV_COLOR_FORMAT_RGB888;
    const int bits_per_pixel = LV_COLOR_FORMAT_GET_BPP(cf);
    uint32_t stride = lv_draw_buf_width_to_stride(width, cf);
    const size_t buffer_sz = stride * height;
    _canvas_buffer = std::make_unique<lv_color_t[]>(buffer_sz);
    lv_canvas_set_buffer(_lv_obj, _canvas_buffer.get(), width, height, cf);
    return std::pair<int,int>(width, height);
  }
  return std::pair<int,int>(0,0);
}

lv_obj_t *LeleColorWheel::createLvObj(LeleObject *lele_parent, lv_obj_t *lv_obj) {

  _lv_obj = LeleObject::createLvObj(lele_parent,
    lv_canvas_create(lele_parent->getLvObj()));
  lv_obj_add_flag(_lv_obj, LV_OBJ_FLAG_CLICKABLE);
  
  std::tie<int,int>(_width, _height) = initCanvas();
  if(_width <= 0 || _height <= 0) {
    LL(DEBUG, LVSIM) << "Failed to initialize canvas";
  }

  for (const auto &[key, token]: _nodes) {
    if (std::holds_alternative<std::unique_ptr<LeleObject>>(token)) {
      auto &value = std::get<std::unique_ptr<LeleObject>>(token);
      value->createLvObj(this);
    }
    // else if (std::holds_alternative<std::string>(token)) {
    //   const std::string &value = std::get<std::string>(token);
    //   if(key == "group") {
    //     _group = (value == "true");
    //   }
    // }
  }
  int bgcolor = 0xffffff;
  auto value = getStyle("bgcolor");
  if(value) {
    bgcolor = std::get<int>(value.value());
  }
  makeColorWheel(_lv_obj, _width, _height, bgcolor);
  
  return _lv_obj;
}

bool LeleColorWheel::eventCallback(LeleEvent &&e) {

  // LL(DEBUG, LVSIM) << "LeleColorWheel::eventCallback " <<
  //   ", event_code: " << e.getLvEvent()->code <<
  //   " " << lv_event_code_get_name(e.getLvEvent()->code);

  switch(e.getLvEvent()->code) {
    case LV_EVENT_CLICKED:{
      GraphicsBackend &backend = GraphicsBackend::getInstance();
      lv_point_t pt = backend.getTouchPoint(_lv_obj);
      if(!isWithinCircle(_width, _height, pt.x, pt.y)) {
        LL(DEBUG, LVSIM) << "LeleColorWheel::LV_EVENT_CLICKED x,y:" << pt.x << "," << pt.y << ", but outside the circle. Ignoring click event";
        return false;
      }
      lv_color32_t color = lv_canvas_get_px(_lv_obj, pt.x, pt.y); 
      LL(DEBUG, LVSIM) << "LeleColorWheel::LV_EVENT_CLICKED x,y:" << pt.x << "," << pt.y << ", rgb: " << 
        std::setfill('0') << std::setw(2) <<
        std::hex << (int)color.red <<
        std::hex << (int)color.green <<
        std::hex << (int)color.blue;
      _rgb = (color.red << 16) | (color.green << 8) | (color.blue);
      for(auto *py_callback:_py_callbacks) {
        if(!pyCallback(py_callback, _rgb)) {
          return false;
        }
      }
      break;
    }
    default:
      break;
  }
  return true;
}

void LeleColorWheel::setColor(int32_t rgb){
  _rgb = rgb;
}

int32_t LeleColorWheel::getColor() const {
  return _rgb;
}

void LeleColorWheel::onColorChanged(PyObject *py_callback) {
  LeleObject::addEventHandler(py_callback);
  // Py_XINCREF(py_callback);
}

bool LeleColorWheel::pyCallback(PyObject *py_callback, int32_t rgb) {

    // LOG(DEBUG, LVSIM, "LeleColorWheel::pyCallback:'%p'\n", py_callback);

    PyObject *py_int = Py_BuildValue("(i)", rgb);
    if(!py_int) {
      LL(WARNING, LVSIM) << "LeleColorWheel::pyCallback could not build py_int from int rgb!";
      return false;
    }
    Py_INCREF(py_int);
    return LeleObject::pyCallback(py_callback, py_int);
}