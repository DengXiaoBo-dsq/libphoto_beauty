#pragma once

#include <cmath>
#include <algorithm>

namespace pbe {

struct RGB {
    float r, g, b;
};

struct HSL {
    float h, s, l;
};

inline float clampf(float v, float lo, float hi) {
    return std::max(lo, std::min(hi, v));
}

RGB rgb_from_u8(unsigned char r, unsigned char g, unsigned char b);
HSL rgb_to_hsl(RGB c);
RGB hsl_to_rgb(HSL hsl);

float srgb_to_linear(float v);
float linear_to_srgb(float v);

float luminance(RGB c);

} // namespace pbe
