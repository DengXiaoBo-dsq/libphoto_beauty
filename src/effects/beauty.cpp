#include "effects/beauty.h"
#include "effects/mask.h"
#include "core/color.h"
#include <cmath>
#include <vector>

namespace pbe::effects {

static void box_blur_rgb(const ImageRGBA8& src, ImageRGBA8& dst, int radius) {
    dst = src;
    radius = std::max(1, std::min(radius, 12));
    const int w = src.width;
    const int h = src.height;
    for (int y = 0; y < h; ++y) {
        uint8_t* drow = dst.row(y);
        for (int x = 0; x < w; ++x) {
            long sum[3] = {0, 0, 0};
            int count = 0;
            for (int yy = std::max(0, y - radius); yy <= std::min(h - 1, y + radius); ++yy) {
                const uint8_t* srow = src.row(yy);
                for (int xx = std::max(0, x - radius); xx <= std::min(w - 1, x + radius); ++xx) {
                    const int i = xx * 4;
                    sum[0] += srow[i + 0];
                    sum[1] += srow[i + 1];
                    sum[2] += srow[i + 2];
                    ++count;
                }
            }
            const int di = x * 4;
            drow[di + 0] = static_cast<uint8_t>(sum[0] / count);
            drow[di + 1] = static_cast<uint8_t>(sum[1] / count);
            drow[di + 2] = static_cast<uint8_t>(sum[2] / count);
        }
    }
}

void apply_beauty(
    ImageRGBA8& image,
    const PBEFaceData* face,
    const PBEBeautyParams& params,
    PBERenderQuality quality
) {
    const float master = clampf(params.strength, 0.0f, 1.0f);
    const float smooth = clampf(params.skin_smooth * master, 0.0f, 1.0f);
    const float tone = clampf(params.skin_tone * master, -1.0f, 1.0f);
    const float bright = clampf(params.skin_bright * master, -1.0f, 1.0f);
    if (smooth < 0.001f && std::fabs(tone) < 0.001f && std::fabs(bright) < 0.001f) {
        return;
    }

    int radius = quality == PBE_QUALITY_FAST ? 2 : (quality == PBE_QUALITY_HIGH ? 3 : 4);
    if (smooth > 0.65f) ++radius;

    ImageRGBA8 blurred;
    box_blur_rgb(image, blurred, radius);

    const bool has_mask = face && face->skin_mask &&
                          face->mask_width == image.width &&
                          face->mask_height == image.height;

    for (int y = 0; y < image.height; ++y) {
        uint8_t* row = image.row(y);
        const uint8_t* brow = blurred.row(y);
        for (int x = 0; x < image.width; ++x) {
            const int i = x * 4;
            const float r = from_u8(row[i + 0]);
            const float g = from_u8(row[i + 1]);
            const float b = from_u8(row[i + 2]);

            float mask = has_mask
                ? sample_mask(face->skin_mask, face->mask_width, face->mask_height, x, y)
                : (fallback_skin_probability(r, g, b) ? 1.0f : 0.0f);

            // Limit smoothing near strong edges to preserve facial contours.
            const float edge = std::fabs(r - from_u8(brow[i + 0])) +
                               std::fabs(g - from_u8(brow[i + 1])) +
                               std::fabs(b - from_u8(brow[i + 2]));
            mask *= clampf(1.0f - edge * 2.0f, 0.0f, 1.0f);

            const float blend = smooth * mask * 0.85f;
            float nr = r + (from_u8(brow[i + 0]) - r) * blend;
            float ng = g + (from_u8(brow[i + 1]) - g) * blend;
            float nb = b + (from_u8(brow[i + 2]) - b) * blend;

            // Gentle hue-preserving skin tone lift in HSL.
            RGB c{nr, ng, nb};
            HSL h = rgb_to_hsl(c);
            const bool warm = h.h >= 5.0f && h.h <= 60.0f && h.s >= 0.08f;
            if (warm) {
                h.h += tone * 3.0f;
                h.s = clampf(h.s * (1.0f - tone * 0.10f), 0.0f, 1.0f);
                h.l = clampf(h.l + bright * 0.08f, 0.0f, 1.0f);
                c = hsl_to_rgb(h);
                nr = c.r; ng = c.g; nb = c.b;
            }

            row[i + 0] = to_u8(nr);
            row[i + 1] = to_u8(ng);
            row[i + 2] = to_u8(nb);
        }
    }
}

} // namespace pbe::effects
