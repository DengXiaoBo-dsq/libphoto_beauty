#include "effects/basic.h"
#include "core/color.h"
#include <cmath>
#include <cstdint>

namespace pbe::effects {

static float pseudo_noise(int x, int y) {
    uint32_t n = static_cast<uint32_t>(x) * 374761393u
               + static_cast<uint32_t>(y) * 668265263u;
    n = (n ^ (n >> 13u)) * 1274126177u;
    n ^= (n >> 16u);
    return static_cast<float>(n & 0xffffu) / 65535.0f;
}

static float smoothstep(float a, float b, float x) {
    x = clampf((x - a) / (b - a), 0.0f, 1.0f);
    return x * x * (3.0f - 2.0f * x);
}

void apply_basic_tone(ImageRGBA8& image, const PBEBeautyParams& params) {
    const float master = clampf(params.strength, 0.0f, 1.0f);
    const float exposure = params.exposure * master;
    const float contrast = params.contrast * master;
    const float highlights = params.highlights * master;
    const float shadows = params.shadows * master;
    const float whites = params.whites * master;
    const float blacks = params.blacks * master;
    const float saturation = params.saturation * master;
    const float vibrance = params.vibrance * master;
    const float temperature = params.temperature * master;
    const float tint = params.tint * master;

    const float exposure_mul = std::pow(2.0f, exposure);
    const float contrast_k = 1.0f + contrast;

    for (int y = 0; y < image.height; ++y) {
        uint8_t* row = image.row(y);
        for (int x = 0; x < image.width; ++x) {
            const int i = x * 4;
            RGB c{
                from_u8(row[i + 0]),
                from_u8(row[i + 1]),
                from_u8(row[i + 2])
            };

            c.r *= exposure_mul;
            c.g *= exposure_mul;
            c.b *= exposure_mul;

            const float y_l = luminance(c);
            if (highlights < 0.0f) {
                const float w = smoothstep(0.55f, 1.0f, y_l);
                const float factor = 1.0f + highlights * w;
                c.r *= factor; c.g *= factor; c.b *= factor;
            } else {
                const float w = smoothstep(0.55f, 1.0f, y_l);
                const float factor = 1.0f + highlights * 0.5f * w;
                c.r *= factor; c.g *= factor; c.b *= factor;
            }

            if (whites != 0.0f) {
                const float w = smoothstep(0.68f, 1.0f, y_l);
                const float gain = 1.0f + whites * 0.65f * w;
                c.r *= gain; c.g *= gain; c.b *= gain;
            }
            if (blacks != 0.0f) {
                const float w = 1.0f - smoothstep(0.0f, 0.28f, y_l);
                const float gain = 1.0f + blacks * 0.50f * w;
                c.r *= gain; c.g *= gain; c.b *= gain;
            }

            if (shadows > 0.0f) {
                const float w = 1.0f - smoothstep(0.12f, 0.55f, y_l);
                const float factor = 1.0f + shadows * w;
                c.r *= factor; c.g *= factor; c.b *= factor;
            } else {
                const float w = 1.0f - smoothstep(0.12f, 0.55f, y_l);
                const float factor = 1.0f + shadows * 0.6f * w;
                c.r *= factor; c.g *= factor; c.b *= factor;
            }

            c.r = (c.r - 0.5f) * contrast_k + 0.5f;
            c.g = (c.g - 0.5f) * contrast_k + 0.5f;
            c.b = (c.b - 0.5f) * contrast_k + 0.5f;

            // Temperature/tint approximation in RGB working space.
            c.r += temperature * 0.08f;
            c.b -= temperature * 0.08f;
            c.g += tint * 0.05f;
            c.r -= tint * 0.02f;
            c.b -= tint * 0.02f;

            RGB base = c;
            const float lum = luminance(base);
            c.r = lum + (c.r - lum) * (1.0f + saturation);
            c.g = lum + (c.g - lum) * (1.0f + saturation);
            c.b = lum + (c.b - lum) * (1.0f + saturation);

            const float mx = std::max({c.r, c.g, c.b});
            const float mn = std::min({c.r, c.g, c.b});
            const float colorfulness = mx - mn;
            const float vibrance_gain = 1.0f + vibrance * (1.0f - colorfulness);
            c.r = lum + (c.r - lum) * vibrance_gain;
            c.g = lum + (c.g - lum) * vibrance_gain;
            c.b = lum + (c.b - lum) * vibrance_gain;

            row[i + 0] = to_u8(c.r);
            row[i + 1] = to_u8(c.g);
            row[i + 2] = to_u8(c.b);
        }
    }
}

void apply_sharpen(ImageRGBA8& image, float amount) {
    amount = clampf(amount, 0.0f, 1.0f);
    if (amount <= 0.001f) return;

    ImageRGBA8 copy = image;
    for (int y = 1; y < image.height - 1; ++y) {
        uint8_t* dst = image.row(y);
        const uint8_t* up = copy.row(y - 1);
        const uint8_t* cur = copy.row(y);
        const uint8_t* dn = copy.row(y + 1);

        for (int x = 1; x < image.width - 1; ++x) {
            const int i = x * 4;
            for (int c = 0; c < 3; ++c) {
                const float center = static_cast<float>(cur[i + c]);
                const float blur = (
                    static_cast<float>(up[i + c]) +
                    static_cast<float>(dn[i + c]) +
                    static_cast<float>(cur[i - 4 + c]) +
                    static_cast<float>(cur[i + 4 + c])
                ) * 0.25f;
                const float v = center + (center - blur) * (1.7f * amount);
                dst[i + c] = static_cast<uint8_t>(clampf(v, 0.0f, 255.0f));
            }
        }
    }
}

void apply_vignette(ImageRGBA8& image, float amount) {
    amount = clampf(amount, 0.0f, 1.0f);
    if (amount <= 0.001f) return;

    const float cx = static_cast<float>(image.width - 1) * 0.5f;
    const float cy = static_cast<float>(image.height - 1) * 0.5f;
    const float invx = 1.0f / std::max(1.0f, cx);
    const float invy = 1.0f / std::max(1.0f, cy);

    for (int y = 0; y < image.height; ++y) {
        uint8_t* row = image.row(y);
        const float ny = (static_cast<float>(y) - cy) * invy;
        for (int x = 0; x < image.width; ++x) {
            const int i = x * 4;
            const float nx = (static_cast<float>(x) - cx) * invx;
            const float d = std::sqrt(nx * nx + ny * ny);
            const float v = 1.0f - amount * clampf((d - 0.25f) / 0.75f, 0.0f, 1.0f);
            row[i + 0] = to_u8(from_u8(row[i + 0]) * v);
            row[i + 1] = to_u8(from_u8(row[i + 1]) * v);
            row[i + 2] = to_u8(from_u8(row[i + 2]) * v);
        }
    }
}

void apply_grain(ImageRGBA8& image, float amount) {
    amount = clampf(amount, 0.0f, 1.0f);
    if (amount <= 0.001f) return;

    for (int y = 0; y < image.height; ++y) {
        uint8_t* row = image.row(y);
        for (int x = 0; x < image.width; ++x) {
            const int i = x * 4;
            const float n = (pseudo_noise(x, y) - 0.5f) * 2.0f;
            const float delta = n * amount * 10.0f;
            for (int c = 0; c < 3; ++c) {
                row[i + c] = static_cast<uint8_t>(
                    clampf(static_cast<float>(row[i + c]) + delta, 0.0f, 255.0f)
                );
            }
        }
    }
}

} // namespace pbe::effects
