#pragma once

#include <cstdint>
#include <string>
#include <vector>

class Mandelbrot {
public:
    enum ColorScheme : int {
        ClassicBlue = 0,
        FireHeat = 1,
        Rainbow = 2,
        Electric = 3
    };

    static bool hasCuda();
    static std::vector<uint8_t> generateBGR(
        int width, int height,
        int max_iterations = 1000,
        double escape_radius_sq = 4.0,
        double real_min = -2.0,
        double real_max = 1.0,
        double imag_min = -1.0,
        double imag_max = 1.0,
        ColorScheme color_scheme = ClassicBlue
    );
    static bool generateToFile(
        const std::string &filename,
        int width, int height,
        int max_iterations = 1000,
        double escape_radius_sq = 4.0,
        double real_min = -2.0,
        double real_max = 1.0,
        double imag_min = -1.0,
        double imag_max = 1.0,
        ColorScheme color_scheme = ClassicBlue
    );
};
