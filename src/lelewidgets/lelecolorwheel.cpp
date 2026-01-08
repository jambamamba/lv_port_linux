#include "lelecolorwheel.h"

#include <lvgl/src/widgets/canvas/lv_canvas.h>
#include <math.h>

LOG_CATEGORY(LVSIM, "LVSIM");

#define PI 3.1415926535897932384626433832795f

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
  auto width_ = getStyle("width");
  auto height_ = getStyle("height");
  int width = 0;
  int height = 0;
  if(width_ && std::holds_alternative<int>(width_.value())) {
    width = std::get<int>(width_.value());
  }
  if(height_ && std::holds_alternative<int>(height_.value())) {
    height = std::get<int>(height_.value());
  }
  if(width > 0 && height > 0) {
    makeColorWheel(width, height);
  }
  return true;
}

lv_obj_t *LeleColorWheel::createLvObj(LeleObject *lele_parent, lv_obj_t *lv_obj) {

  _lv_obj = LeleObject::createLvObj(lele_parent,
    lv_canvas_create(lele_parent->getLvObj()));

  return _lv_obj;
}

namespace {
// Structure to hold RGB color values (0-1 range for this example)
struct RgbColor {
    float r;
    float g;
    float b;
};

// Function to convert HSV to RGB
RgbColor HsvToRgb(float h, float s, float v) {
    RgbColor rgb;
    float f, p, q, t;
    int i;

    // If saturation is 0, the color is achromatic (grey)
    if (s == 0.0f) {
        rgb.r = rgb.g = rgb.b = v;
        return rgb;
    }

    // Wrap hue to the range [0, 360] if necessary
    h = std::fmod(h, 360.0f);
    if (h < 0.0f) {
        h += 360.0f;
    }

    h /= 60.0f;           // sector 0 to 5
    i = static_cast<int>(std::floor(h)); // largest integer <= h
    f = h - i;            // fractional part of h

    p = v * (1.0f - s);
    q = v * (1.0f - s * f);
    t = v * (1.0f - s * (1.0f - f));

    switch (i) {
        case 0:
            rgb.r = v;
            rgb.g = t;
            rgb.b = p;
            break;
        case 1:
            rgb.r = q;
            rgb.g = v;
            rgb.b = p;
            break;
        case 2:
            rgb.r = p;
            rgb.g = v;
            rgb.b = t;
            break;
        case 3:
            rgb.r = p;
            rgb.g = q;
            rgb.b = v;
            break;
        case 4:
            rgb.r = t;
    }
    return rgb;
  }
}

void LeleColorWheel::makeColorWheel(int width, int height) const {

    // Define the center coordinates of the canvas
    int center_x = width / 2;
    int center_y = height / 2;

    // Define the maximum radius for the color wheel
    int max_radius = std::min(center_x, center_y) * 0.9f; // Use 90% of the smallest half for padding

    // Loop through all pixels in the canvas
    for(int y = 0; y < height; ++y){
      for(int x = 0; y < width; ++x){

        // Calculate the distance from the pixel to the center (using Pythagorean theorem)
        // dx = delta x, dy = delta y
        int dx = x - center_x;
        int dy = y - center_y;
        float distance = pow(dx * dx + dy * dy, .5f);

        // Check if the pixel is within the main circular area
        if (distance <= max_radius) {

            // Calculate the angle (hue) using the arctangent function (atan2)
            // The result is typically in the range [-PI, PI]
            float angle_radians = atan2(dy, dx);

            // Convert the angle from radians to degrees and adjust to a 0-360 range for HSL/HSV
            int angle_degrees = (angle_radians * 180 / PI); // Convert to degrees (-180 to 180)
            int hue = (angle_degrees + 360) % 360;      // Map to 0-360 range

            // Determine saturation: 100% at the edge, 0% at the center
            // The distance is mapped from [0, max_radius] to [0, 100]
            int saturation = (distance / max_radius) * 100;

            // Set brightness to maximum for a full-intensity wheel
            int brightness = 100;

            // Convert the calculated HSL/HSV values to an RGB color
            // This requires a separate conversion function
            RgbColor rgb = HsvToRgb(hue, saturation, brightness);

            // Set the pixel's color on the canvas
            // SET Pixel(x, y) TO rgb_color
            lv_color_t c = lv_color_make(rgb.r, rgb.g, rgb.b);
            lv_canvas_set_px(_lv_obj, x, y, c, 0);

        }
        else {
            // If outside the wheel, set a background color (e.g., white)
            // SET Pixel(x, y) TO White // Or transparent, depending on implementation
            lv_color_t c = lv_color_make(0xff, 0xff, 0xff);
            lv_canvas_set_px(_lv_obj, x, y, c, 0);
        }
      }
    }
}