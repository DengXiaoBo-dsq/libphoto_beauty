#include "beauty/skin.h"
#include "effects/mask.h"
#include "core/color.h"
#include <algorithm>
#include <cmath>

namespace pbe::beauty {

static float color_weight(float dr, float dg, float db, float sigma) {
    return std::exp(-(dr * dr + dg * dg + db * db) / (2.0f * sigma * sigma));
}

void apply_skin(
    ImageRGBA8& image,
    const PBEFaceData* face,
    const PBEBeautyParams& params,
    PBERenderQuality quality
) {
    const float smooth = std::clamp(params.skin_smooth * params.strength, 0.0f, 1.0f);
    const float tone = params.skin_tone * params.strength;
    const float bright = params.skin_bright * params.strength;
    if (smooth < 1e-4f && std::fabs(tone) < 1e-4f && std::fabs(bright) < 1e-4f) return;

    const bool has_mask = face && face->skin_mask &&
                          face->mask_width == image.width && face->mask_height == image.height;

    if (smooth >= 1e-4f) {
        ImageRGBA8 src = image;
        const int radius = quality == PBE_QUALITY_FAST ? 1 :
                           (quality == PBE_QUALITY_HIGH ? 2 : 3);
        const float sigma_color = quality == PBE_QUALITY_ULTRA ? 0.16f : 0.20f;

        for (int y = 0; y < image.height; ++y) {
            for (int x = 0; x < image.width; ++x) {
                const int i = x * 4;
                const float r = src.row(y)[i + 0] / 255.0f;
                const float g = src.row(y)[i + 1] / 255.0f;
                const float b = src.row(y)[i + 2] / 255.0f;

                const float m = has_mask
                    ? effects::sample_mask(face->skin_mask, image.width, image.height, x, y)
                    : (effects::fallback_skin_probability(r, g, b) ? 1.0f : 0.0f);
                if (m < 0.01f) continue;

                float sr = 0.0f, sg = 0.0f, sb = 0.0f, sw = 0.0f;
                for (int yy = std::max(0, y - radius);
                     yy <= std::min(image.height - 1, y + radius); ++yy) {
                    for (int xx = std::max(0, x - radius);
                         xx <= std::min(image.width - 1, x + radius); ++xx) {
                        const int j = xx * 4;
                        const float rr = src.row(yy)[j + 0] / 255.0f;
                        const float gg = src.row(yy)[j + 1] / 255.0f;
                        const float bb = src.row(yy)[j + 2] / 255.0f;
                        const float spatial = 1.0f / (
                            1.0f + std::abs(static_cast<float>(xx - x)) +
                            std::abs(static_cast<float>(yy - y)));
                        const float chroma = color_weight(
                            r - rr, g - gg, b - bb, sigma_color);
                        const float w = spatial * chroma;
                        sr += rr * w;
                        sg += gg * w;
                        sb += bb * w;
                        sw += w;
                    }
                }

                if (sw > 1e-6f) {
                    const float nr = sr / sw;
                    const float ng = sg / sw;
                    const float nb = sb / sw;
                    const float blend = smooth * m * 0.90f;
                    image.row(y)[i + 0] = to_u8(r + (nr - r) * blend);
                    image.row(y)[i + 1] = to_u8(g + (ng - g) * blend);
                    image.row(y)[i + 2] = to_u8(b + (nb - b) * blend);
                }
            }
        }
    }

    if (std::fabs(tone) < 1e-4f && std::fabs(bright) < 1e-4f) return;

    for (int y = 0; y < image.height; ++y) {
        for (int x = 0; x < image.width; ++x) {
            const int i = x * 4;
            const float r = image.row(y)[i + 0] / 255.0f;
            const float g = image.row(y)[i + 1] / 255.0f;
            const float b = image.row(y)[i + 2] / 255.0f;
            const float m = has_mask
                ? effects::sample_mask(face->skin_mask, image.width, image.height, x, y)
                : (effects::fallback_skin_probability(r, g, b) ? 1.0f : 0.0f);
            if (m < 0.01f) continue;

            RGB c{r, g, b};
            HSL h = rgb_to_hsl(c);
            if (h.h >= 5.0f && h.h <= 60.0f && h.s >= 0.07f) {
                h.h += tone * 3.0f;
                h.s = std::clamp(h.s * (1.0f - tone * 0.08f), 0.0f, 1.0f);
                h.l = std::clamp(h.l + bright * 0.06f, 0.0f, 1.0f);
                c = hsl_to_rgb(h);
                image.row(y)[i + 0] = to_u8(c.r * m + r * (1.0f - m));
                image.row(y)[i + 1] = to_u8(c.g * m + g * (1.0f - m));
                image.row(y)[i + 2] = to_u8(c.b * m + b * (1.0f - m));
            }
        }
    }
}

} // namespace pbe::beauty
