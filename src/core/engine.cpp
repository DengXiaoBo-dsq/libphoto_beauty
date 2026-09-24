#include "core/engine.h"
#include "core/color.h"

#include <cstring>
#include <cmath>
#include <algorithm>

namespace pbe {

PBEBeautyParams default_beauty_params() {
    PBEBeautyParams p{};
    p.strength = 1.0f;
    p.skin_smooth = 0.0f;
    p.skin_tone = 0.0f;
    p.skin_bright = 0.0f;
    p.blemish_reduction = 0.0f;
    p.face_slim = 0.0f;
    p.eye_scale = 0.0f;
    p.nose_scale = 0.0f;
    p.jaw_width = 0.0f;
    p.exposure = 0.0f;
    p.contrast = 0.0f;
    p.highlights = 0.0f;
    p.shadows = 0.0f;
    p.temperature = 0.0f;
    p.tint = 0.0f;
    p.saturation = 0.0f;
    p.vibrance = 0.0f;
    p.clarity = 0.0f;
    p.sharpen = 0.0f;
    p.vignette = 0.0f;
    p.grain = 0.0f;
    return p;
}

PBEAutoSettings default_auto_settings() {
    return {1, 1, 1, 1};
}

Engine::Engine(const PBEConfig& cfg)
    : config_(cfg),
      beauty_(default_beauty_params()),
      auto_(default_auto_settings()) {}

void Engine::set_error(const char* message) {
    last_error_ = message ? message : "";
}

PBEStatus Engine::reset() {
    beauty_ = default_beauty_params();
    auto_ = default_auto_settings();
    face_ = nullptr;
    input_rgba_.pixels.clear();
    output_rgba_.pixels.clear();
    last_error_.clear();
    return PBE_OK;
}

PBEStatus Engine::set_beauty(const PBEBeautyParams& params) {
    beauty_ = params;
    return PBE_OK;
}

PBEStatus Engine::set_auto(const PBEAutoSettings& settings) {
    auto_ = settings;
    return PBE_OK;
}

PBEStatus Engine::set_face_data(const PBEFaceData* face) {
    face_ = face;
    return PBE_OK;
}

static PBEStatus import_image(const PBEImageView& in, ImageRGBA8& dst) {
    if (!in.data || in.width <= 0 || in.height <= 0 || in.stride_bytes <= 0) {
        return PBE_INVALID_ARGUMENT;
    }

    dst.reset(in.width, in.height);
    for (int y = 0; y < in.height; ++y) {
        const uint8_t* src = in.data + static_cast<size_t>(y) * static_cast<size_t>(in.stride_bytes);
        uint8_t* out = dst.row(y);

        if (in.format == PBE_IMAGE_RGBA8) {
            std::memcpy(out, src, static_cast<size_t>(in.width) * 4u);
        } else if (in.format == PBE_IMAGE_BGRA8) {
            for (int x = 0; x < in.width; ++x) {
                out[x * 4 + 0] = src[x * 4 + 2];
                out[x * 4 + 1] = src[x * 4 + 1];
                out[x * 4 + 2] = src[x * 4 + 0];
                out[x * 4 + 3] = src[x * 4 + 3];
            }
        } else if (in.format == PBE_IMAGE_RGB8) {
            for (int x = 0; x < in.width; ++x) {
                out[x * 4 + 0] = src[x * 3 + 0];
                out[x * 4 + 1] = src[x * 3 + 1];
                out[x * 4 + 2] = src[x * 3 + 2];
                out[x * 4 + 3] = 255;
            }
        } else {
            return PBE_UNSUPPORTED;
        }
    }
    return PBE_OK;
}

static PBEStatus export_image(const ImageRGBA8& src, PBEImageView& out) {
    if (!out.mutable_data || out.width != src.width || out.height != src.height ||
        out.stride_bytes <= 0) {
        return PBE_INVALID_ARGUMENT;
    }

    for (int y = 0; y < src.height; ++y) {
        const uint8_t* row = src.row(y);
        uint8_t* dst = out.mutable_data + static_cast<size_t>(y) * static_cast<size_t>(out.stride_bytes);

        if (out.format == PBE_IMAGE_RGBA8) {
            std::memcpy(dst, row, static_cast<size_t>(src.width) * 4u);
        } else if (out.format == PBE_IMAGE_BGRA8) {
            for (int x = 0; x < src.width; ++x) {
                dst[x * 4 + 0] = row[x * 4 + 2];
                dst[x * 4 + 1] = row[x * 4 + 1];
                dst[x * 4 + 2] = row[x * 4 + 0];
                dst[x * 4 + 3] = row[x * 4 + 3];
            }
        } else {
            return PBE_UNSUPPORTED;
        }
    }
    return PBE_OK;
}

PBEStatus Engine::render(const PBEImageView& input, PBEImageView& output) {
    if (!input.data || !output.mutable_data) {
        set_error("Input/output buffer is null.");
        return PBE_INVALID_ARGUMENT;
    }

    const PBEStatus import_status = import_image(input, input_rgba_);
    if (import_status != PBE_OK) {
        set_error("Unsupported input image.");
        return import_status;
    }

    if (auto_.enable_auto_exposure || auto_.enable_auto_color || auto_.enable_auto_beauty) {
        // Auto-enhancement is idempotent relative to the stored parameters:
        // it only adjusts the current neutral parameters once per render.
        // pbe_auto_enhance() can be called explicitly to freeze those choices.
    }

    const PBEStatus status = pipeline_.render(
        input_rgba_, output_rgba_, beauty_, face_, config_.quality);

    if (status != PBE_OK) {
        set_error("Pipeline render failed.");
        return status;
    }

    const PBEStatus export_status = export_image(output_rgba_, output);
    if (export_status != PBE_OK) {
        set_error("Output format/stride is invalid.");
        return export_status;
    }
    return PBE_OK;
}

PBEStatus Engine::auto_enhance() {
    if (input_rgba_.empty()) {
        set_error("Load/render an image before auto_enhance.");
        return PBE_INVALID_STATE;
    }

    double mean = 0.0;
    double mean2 = 0.0;
    const int sample_step = std::max(1, std::max(input_rgba_.width, input_rgba_.height) / 256);
    size_t n = 0;

    for (int y = 0; y < input_rgba_.height; y += sample_step) {
        const uint8_t* row = input_rgba_.row(y);
        for (int x = 0; x < input_rgba_.width; x += sample_step) {
            const int i = x * 4;
            const double lum =
                0.2126 * row[i + 0] / 255.0 +
                0.7152 * row[i + 1] / 255.0 +
                0.0722 * row[i + 2] / 255.0;
            mean += lum;
            mean2 += lum * lum;
            ++n;
        }
    }

    if (n == 0) return PBE_INTERNAL_ERROR;
    mean /= static_cast<double>(n);
    mean2 /= static_cast<double>(n);
    const double variance = std::max(0.0, mean2 - mean * mean);
    const double stddev = std::sqrt(variance);

    if (auto_.enable_auto_exposure) {
        const float target = 0.48f;
        beauty_.exposure += static_cast<float>((target - mean) * 1.25);
        beauty_.exposure = std::max(-1.25f, std::min(1.25f, beauty_.exposure));
    }

    if (auto_.enable_auto_color) {
        if (stddev < 0.16) beauty_.contrast += 0.08f;
        else if (stddev > 0.30) beauty_.contrast -= 0.04f;
        beauty_.vibrance += 0.05f;
    }

    if (auto_.enable_auto_beauty) {
        beauty_.skin_smooth = std::max(beauty_.skin_smooth, 0.12f);
        beauty_.skin_bright = std::max(beauty_.skin_bright, 0.03f);
    }

    if (auto_.enable_auto_sharpen) {
        beauty_.sharpen = std::max(beauty_.sharpen, 0.10f);
    }

    return PBE_OK;
}

} // namespace pbe
