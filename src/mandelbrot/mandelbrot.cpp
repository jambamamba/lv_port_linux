#include "mandelbrot.h"

#include "mandelbrot.h"

#include <chrono>
#include <cmath>
#include <complex>
#include <cstdint>
#include <iostream>
#include <map>
#include <vector>

#include <debug_logger/debug_logger.h>
#include <image_builder/image_builder.h>
#include <lv_image_converter/lv_image_converter.h>

#ifdef USE_CUDA
#include <cuda_runtime_api.h>
#endif

LOG_CATEGORY(MANDEL, "MANDEL");

using Complex = std::complex<double>;

bool Mandelbrot::hasCuda() {
#ifdef USE_CUDA
    int deviceCount = 0;
    if(cudaGetDeviceCount(&deviceCount) == cudaSuccess && deviceCount > 0) {
        return true;
    }
#endif
    return false;
}

namespace {

using Complex = std::complex<double>;

std::vector<Complex> generateComplexSet(
    int width, int height,
    double real_min, double real_max,
    double imag_min, double imag_max
) {
    std::vector<Complex> coords;
    coords.reserve(width * height);
    for(int y = 0; y < height; ++y) {
        double imag_frac = static_cast<double>(y) / (static_cast<double>(height) - 1.0);
        double imag = imag_min + (imag_max - imag_min) * imag_frac;
        for(int x = 0; x < width; ++x) {
            double real_frac = static_cast<double>(x) / (static_cast<double>(width) - 1.0);
            double real = real_min + (real_max - real_min) * real_frac;
            coords.emplace_back(real, imag);
        }
    }
    return coords;
}

double mandelbrotIteration(Complex c, int max_iterations, double escape_radius_sq) {
    Complex z(0.0, 0.0);
    Complex dz(1.0, 0.0);
    for(int i = 0; i < max_iterations; ++i) {
        double mag2 = std::norm(z);
        if(mag2 > escape_radius_sq) {
            double mag_z = std::abs(z);
            double mag_dz = std::abs(dz);
            return std::log(mag2) * mag_z / mag_dz;
        }
        dz = 2.0 * z * dz + Complex(1.0, 0.0);
        z = z * z + c;
    }
    return 0.0;
}

struct Palette {
    const double *stops;
    const uint8_t (*colors)[3];
    int n_stops;
};

static const double fire_stops[] = {0.00, 0.20, 0.40, 0.60, 0.80, 1.00};
static const uint8_t fire_colors[][3] = {
    {60, 0, 0},      // dark red
    {255, 30, 0},    // bright red
    {255, 120, 0},   // orange
    {255, 210, 0},   // orange-yellow
    {255, 255, 120}, // yellow-white
    {255, 255, 255}, // white
};

static const double classic_stops[] = {0.00, 0.20, 0.40, 0.60, 0.80, 1.00};
static const uint8_t classic_colors[][3] = {
    {0, 0, 0},       // black
    {30, 0, 120},    // deep blue
    {0, 100, 255},   // blue
    {0, 255, 200},   // cyan
    {200, 255, 50},  // lime
    {255, 255, 255}, // white
};

static const double rainbow_stops[] = {0.00, 0.15, 0.30, 0.45, 0.60, 0.75, 0.90, 1.00};
static const uint8_t rainbow_colors[][3] = {
    {0, 0, 0},       // black
    {120, 0, 200},   // violet
    {0, 60, 255},    // blue
    {0, 200, 80},    // green
    {100, 255, 0},   // lime
    {255, 200, 0},   // yellow
    {255, 50, 0},    // red
    {255, 255, 255}, // white
};

static const double electric_stops[] = {0.00, 0.20, 0.40, 0.60, 0.80, 1.00};
static const uint8_t electric_colors[][3] = {
    {0, 0, 0},       // black
    {0, 255, 255},   // cyan
    {255, 0, 255},   // magenta
    {255, 255, 0},   // yellow
    {0, 255, 128},   // spring green
    {255, 255, 255}, // white
};

static const Palette palettes[] = {
    {classic_stops, classic_colors, sizeof(classic_stops) / sizeof(classic_stops[0])},
    {fire_stops, fire_colors, sizeof(fire_stops) / sizeof(fire_stops[0])},
    {rainbow_stops, rainbow_colors, sizeof(rainbow_stops) / sizeof(rainbow_stops[0])},
    {electric_stops, electric_colors, sizeof(electric_stops) / sizeof(electric_stops[0])},
};

std::vector<uint8_t> toBGR(const std::vector<double> &data, int width, int height,
                           Mandelbrot::ColorScheme color_scheme) {
    if(data.empty()) {
        LOG(WARNING, MANDEL, "No mandelbrot data to render\n");
        return std::vector<uint8_t>();
    }

    std::map<uint32_t, uint32_t> hist;
    uint32_t min_val = UINT32_MAX, max_val = 0;
    std::vector<uint32_t> ivalues;
    ivalues.reserve(data.size());

    for(double v : data) {
        if(v == 0.0) {
            ivalues.push_back(0);
            hist[0]++;
            min_val = 0;
            continue;
        }
        uint32_t iv = static_cast<uint32_t>(-std::log2(v));
        ivalues.push_back(iv);
        hist[iv]++;
        if(iv < min_val) min_val = iv;
        if(iv > max_val) max_val = iv;
    }

    // log-frequency histogram equalization
    uint32_t sum_log = 0;
    for(auto &[k, v] : hist) {
        v = (v > 0) ? static_cast<uint32_t>(std::log2(static_cast<double>(v))) : 0;
        sum_log += v;
    }

    // build cumulative frequency map
    std::map<uint32_t, uint32_t> cumul;
    uint32_t running = 0;
    for(auto &[k, v] : hist) {
        cumul[k] = running;
        running += v;
    }

    int cs = static_cast<int>(color_scheme);
    if(cs < 0 || cs >= 4) cs = 0;
    const auto &pal = palettes[cs];
    const double *stops = pal.stops;
    const uint8_t (*colors)[3] = pal.colors;
    int n_stops = pal.n_stops;

    std::vector<uint8_t> pixels;
    pixels.reserve(data.size() * 3);
    for(uint32_t iv : ivalues) {
        if(iv == 0) {
            pixels.push_back(0);   // B
            pixels.push_back(0);   // G
            pixels.push_back(0);   // R
            continue;
        }
        double t = sum_log > 0
            ? static_cast<double>(cumul[iv]) / static_cast<double>(sum_log)
            : 0.0;

        // find the two surrounding stops and interpolate
        uint8_t r, g, b;
        if(t <= stops[0]) { r = colors[0][0]; g = colors[0][1]; b = colors[0][2]; }
        else if(t >= stops[n_stops - 1]) { r = colors[n_stops-1][0]; g = colors[n_stops-1][1]; b = colors[n_stops-1][2]; }
        else {
            int i = 0;
            for(; i < n_stops - 1; i++) {
                if(t >= stops[i] && t < stops[i + 1]) break;
            }
            double frac = (t - stops[i]) / (stops[i + 1] - stops[i]);
            r = static_cast<uint8_t>(colors[i][0] + (colors[i+1][0] - colors[i][0]) * frac);
            g = static_cast<uint8_t>(colors[i][1] + (colors[i+1][1] - colors[i][1]) * frac);
            b = static_cast<uint8_t>(colors[i][2] + (colors[i+1][2] - colors[i][2]) * frac);
        }
        pixels.push_back(b);  // B
        pixels.push_back(g);  // G
        pixels.push_back(r);  // R
    }
    return pixels;
}

} // anonymous namespace

#ifdef USE_CUDA
std::vector<double> pickMethod(
    const std::vector<Complex> &coords,
    int max_iterations, double escape_radius_sq
);
#else
std::vector<double> pickMethod(
    const std::vector<Complex> &coords,
    int max_iterations, double escape_radius_sq
) {
    LOG(WARNING, MANDEL, "CUDA not compiled, nothing rendered\n");
    //todo: OpenGL Compute Shaders (Easiest for Pi 4 and 5)
    //or Vulkan Kompute (Most Modern & High Performance)
    return std::vector<double>();
}
#endif

std::vector<uint8_t> Mandelbrot::generateBGR(
    int width, int height,
    int max_iterations, double escape_radius_sq,
    double real_min, double real_max,
    double imag_min, double imag_max,
    ColorScheme color_scheme
) {
    LOG(DEBUG, MANDEL, "generateBGR(%dx%d, iter=%d, r=[%0.3f,%0.3f] i=[%0.3f,%0.3f], scheme=%d)\n",
        width, height, max_iterations, real_min, real_max, imag_min, imag_max, (int)color_scheme);
    auto coords = generateComplexSet(width, height, real_min, real_max, imag_min, imag_max);
    LOG(DEBUG, MANDEL, "generateBGR: generated %zu coords\n", coords.size());
    auto results = pickMethod(coords, max_iterations, escape_radius_sq);
    LOG(DEBUG, MANDEL, "generateBGR: pickMethod returned %zu results\n", results.size());
    auto bgr = toBGR(results, width, height, color_scheme);
    LOG(DEBUG, MANDEL, "generateBGR: returning %zu bytes\n", bgr.size());
    return bgr;
}

bool Mandelbrot::generateToFile(
    const std::string &filename, int width, int height,
    int max_iterations, double escape_radius_sq,
    double real_min, double real_max,
    double imag_min, double imag_max,
    ColorScheme color_scheme
) {
    auto bgr = generateBGR(width, height, max_iterations, escape_radius_sq,
                           real_min, real_max, imag_min, imag_max, color_scheme);
    if(bgr.empty()) return false;

    auto img_dsc = LeleImageConverter::generateImgDsc(width, height, 3);
    if(!img_dsc) {
        LOG(WARNING, MANDEL, "Failed to create image descriptor\n");
        return false;
    }

    uint8_t *data = const_cast<uint8_t*>(img_dsc->get()->data);
    std::copy(bgr.begin(), bgr.end(), data);
    LeleImageConverter::saveGdImage(filename, img_dsc->get());
    LOG(DEBUG, MANDEL, "Saved mandelbrot to %s (%dx%d)\n", filename.c_str(), width, height);
    return true;
}
