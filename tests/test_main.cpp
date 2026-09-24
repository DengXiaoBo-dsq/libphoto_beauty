#include "pbe/pbe.h"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <vector>

int main() {
    PBEConfig cfg{};
    cfg.quality = PBE_QUALITY_HIGH;
    cfg.prefer_gpu = 0;

    PBEHandle h = nullptr;
    assert(pbe_create(&cfg, &h) == PBE_OK);
    assert(h != nullptr);

    constexpr int W = 32;
    constexpr int H = 32;
    std::vector<uint8_t> input(W * H * 4);
    std::vector<uint8_t> output(W * H * 4);

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            const int i = (y * W + x) * 4;
            input[i + 0] = static_cast<uint8_t>(x * 8);
            input[i + 1] = static_cast<uint8_t>(y * 8);
            input[i + 2] = 128;
            input[i + 3] = 255;
        }
    }

    PBEBeautyParams params{};
    assert(pbe_get_beauty_params(h, &params) == PBE_OK);
    params.strength = 1.0f;
    params.exposure = 0.15f;
    params.contrast = 0.05f;
    params.skin_smooth = 0.15f;
    params.sharpen = 0.1f;
    assert(pbe_set_beauty_params(h, &params) == PBE_OK);

    PBEImageView in{};
    in.data = input.data();
    in.width = W;
    in.height = H;
    in.stride_bytes = W * 4;
    in.format = PBE_IMAGE_RGBA8;

    PBEImageView out{};
    out.mutable_data = output.data();
    out.width = W;
    out.height = H;
    out.stride_bytes = W * 4;
    out.format = PBE_IMAGE_RGBA8;

    assert(pbe_render_rgba8(h, &in, &out) == PBE_OK);
    assert(output[0] != 0 || output[1] != 0 || output[2] != 0);

    pbe_destroy(h);
    std::puts("PhotoBeautyEngine tests passed.");
    return 0;
}
