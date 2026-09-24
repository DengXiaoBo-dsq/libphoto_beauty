#include "pbe/pbe.h"
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <vector>

static void fill_test_image(std::vector<uint8_t>& img, int w, int h) {
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const int i = (y * w + x) * 4;
            img[i + 0] = static_cast<uint8_t>((x * 7 + y * 3) % 256);
            img[i + 1] = static_cast<uint8_t>((y * 7 + 48) % 256);
            img[i + 2] = static_cast<uint8_t>(110 + ((x + y) % 70));
            img[i + 3] = 255;
        }
    }
}

int main() {
    PBEConfig cfg{};
    cfg.quality = PBE_QUALITY_HIGH;
    cfg.prefer_gpu = 0;

    PBEHandle h = nullptr;
    assert(pbe_create(&cfg, &h) == PBE_OK);
    assert(h != nullptr);

    constexpr int W = 64;
    constexpr int H = 48;
    std::vector<uint8_t> input(W * H * 4);
    std::vector<uint8_t> output(W * H * 4, 0);
    fill_test_image(input, W, H);

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

    PBESceneStats stats{};
    assert(pbe_analyze_rgba8(h, &in, &stats) == PBE_OK);
    assert(stats.mean_luma > 0.0f && stats.mean_luma < 1.0f);
    assert(stats.contrast >= 0.0f);

    assert(pbe_apply_preset(h, PBE_PRESET_NATURAL_PORTRAIT, 1.0f) == PBE_OK);
    PBEBeautyParams params{};
    assert(pbe_get_beauty_params(h, &params) == PBE_OK);
    assert(params.skin_smooth > 0.0f);

    std::vector<PBELandmark> landmarks = {
        {0.36f, 0.42f, 0.0f},
        {0.64f, 0.42f, 0.0f},
        {0.50f, 0.52f, 0.0f},
        {0.50f, 0.68f, 0.0f},
        {0.50f, 0.36f, 0.0f}
    };
    std::vector<float> skin_mask(static_cast<size_t>(W) * static_cast<size_t>(H), 0.0f);
    std::vector<float> person_mask(static_cast<size_t>(W) * static_cast<size_t>(H), 0.0f);
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            const float dx = (static_cast<float>(x) / float(W - 1)) - 0.5f;
            const float dy = (static_cast<float>(y) / float(H - 1)) - 0.52f;
            const float d = std::sqrt(dx * dx + dy * dy);
            const float m = d < 0.30f ? 1.0f : 0.0f;
            skin_mask[static_cast<size_t>(y * W + x)] = m;
            person_mask[static_cast<size_t>(y * W + x)] = d < 0.42f ? 1.0f : 0.0f;
        }
    }
    PBEFaceData face{};
    face.landmarks = landmarks.data();
    face.landmark_count = landmarks.size();
    face.skin_mask = skin_mask.data();
    face.person_mask = person_mask.data();
    face.mask_width = W;
    face.mask_height = H;
    assert(pbe_set_face_data(h, &face) == PBE_OK);

    params.face_slim = 0.08f;
    params.eye_scale = 0.05f;
    params.skin_smooth = 0.18f;
    params.local_brightness = 0.10f;
    params.sharpen = 0.08f;
    assert(pbe_set_beauty_params(h, &params) == PBE_OK);
    assert(pbe_render_rgba8(h, &in, &out) == PBE_OK);

    bool changed = false;
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] != output[i]) { changed = true; break; }
    }
    assert(changed);

    assert(pbe_reset(h) == PBE_OK);
    pbe_destroy(h);

    std::puts("PhotoBeautyEngine v0.2 tests passed.");
    return 0;
}
