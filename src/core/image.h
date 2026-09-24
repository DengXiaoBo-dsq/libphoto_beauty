#pragma once

#include <cstdint>
#include <vector>
#include <algorithm>

namespace pbe {

struct ImageRGBA8 {
    int width = 0;
    int height = 0;
    int stride = 0;
    std::vector<uint8_t> pixels;

    bool empty() const { return width <= 0 || height <= 0 || pixels.empty(); }

    void reset(int w, int h) {
        width = w;
        height = h;
        stride = w * 4;
        pixels.assign(static_cast<size_t>(stride) * static_cast<size_t>(height), 0);
    }

    uint8_t* row(int y) {
        return pixels.data() + static_cast<size_t>(y) * static_cast<size_t>(stride);
    }

    const uint8_t* row(int y) const {
        return pixels.data() + static_cast<size_t>(y) * static_cast<size_t>(stride);
    }
};

inline float clamp01(float v) {
    return std::max(0.0f, std::min(1.0f, v));
}

inline uint8_t to_u8(float v) {
    v = clamp01(v);
    return static_cast<uint8_t>(v * 255.0f + 0.5f);
}

inline float from_u8(uint8_t v) {
    return static_cast<float>(v) / 255.0f;
}

} // namespace pbe
