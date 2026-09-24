#include "core/color.h"
#include <algorithm>

namespace pbe {

RGB rgb_from_u8(unsigned char r, unsigned char g, unsigned char b) {
    return {r / 255.0f, g / 255.0f, b / 255.0f};
}

HSL rgb_to_hsl(RGB c) {
    const float maxc = std::max({c.r, c.g, c.b});
    const float minc = std::min({c.r, c.g, c.b});
    const float d = maxc - minc;
    const float l = (maxc + minc) * 0.5f;

    if (d < 1e-6f) {
        return {0.0f, 0.0f, l};
    }

    const float s = d / (1.0f - std::fabs(2.0f * l - 1.0f));
    float h = 0.0f;

    if (maxc == c.r) {
        h = 60.0f * std::fmod(((c.g - c.b) / d), 6.0f);
    } else if (maxc == c.g) {
        h = 60.0f * (((c.b - c.r) / d) + 2.0f);
    } else {
        h = 60.0f * (((c.r - c.g) / d) + 4.0f);
    }

    if (h < 0.0f) h += 360.0f;
    return {h, clampf(s, 0.0f, 1.0f), l};
}

static float hue_to_rgb(float p, float q, float t) {
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}

RGB hsl_to_rgb(HSL hsl) {
    if (hsl.s <= 1e-6f) {
        return {hsl.l, hsl.l, hsl.l};
    }

    const float q = hsl.l < 0.5f
        ? hsl.l * (1.0f + hsl.s)
        : hsl.l + hsl.s - hsl.l * hsl.s;
    const float p = 2.0f * hsl.l - q;
    const float h = hsl.h / 360.0f;

    return {
        hue_to_rgb(p, q, h + 1.0f / 3.0f),
        hue_to_rgb(p, q, h),
        hue_to_rgb(p, q, h - 1.0f / 3.0f)
    };
}

float srgb_to_linear(float v) {
    return v <= 0.04045f
        ? v / 12.92f
        : std::pow((v + 0.055f) / 1.055f, 2.4f);
}

float linear_to_srgb(float v) {
    v = std::max(0.0f, v);
    return v <= 0.0031308f
        ? 12.92f * v
        : 1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f;
}

float luminance(RGB c) {
    return 0.2126f * c.r + 0.7152f * c.g + 0.0722f * c.b;
}

} // namespace pbe
