#include "effects/mask.h"
#include <algorithm>

namespace pbe::effects {

float sample_mask(const float* mask, int width, int height, int x, int y) {
    if (!mask || width <= 0 || height <= 0) return 0.0f;
    x = std::max(0, std::min(width - 1, x));
    y = std::max(0, std::min(height - 1, y));
    return std::max(0.0f, std::min(1.0f, mask[y * width + x]));
}

bool fallback_skin_probability(float r, float g, float b) {
    // Conservative YCbCr-inspired fallback. Production should replace with
    // a learned skin-parsing model through PBEFaceData::skin_mask.
    const float mx = std::max({r, g, b});
    const float mn = std::min({r, g, b});
    const float chroma = mx - mn;
    const float y = 0.299f * r + 0.587f * g + 0.114f * b;
    const float cb = 128.0f + (-0.168736f * r - 0.331264f * g + 0.5f * b) * 255.0f;
    const float cr = 128.0f + (0.5f * r - 0.418688f * g - 0.081312f * b) * 255.0f;
    return y > 0.15f && y < 0.97f &&
           cb > 77.0f && cb < 132.0f &&
           cr > 125.0f && cr < 173.0f &&
           chroma > 0.04f && r >= g * 0.92f;
}

} // namespace pbe::effects
