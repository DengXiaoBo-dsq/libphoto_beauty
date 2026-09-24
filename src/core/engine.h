#pragma once

#include "pbe/pbe.h"
#include "core/image.h"
#include "core/pipeline.h"

#include <string>

namespace pbe {

class Engine {
public:
    explicit Engine(const PBEConfig& cfg);
    ~Engine() = default;

    PBEStatus reset();
    PBEStatus set_beauty(const PBEBeautyParams& params);
    PBEStatus set_auto(const PBEAutoSettings& settings);
    PBEStatus set_advanced_color(const PBEAdvancedColor& params);
    PBEStatus set_ai_provider(const PBEAIProvider* provider);
    const PBEAdvancedColor& advanced_color() const { return advanced_color_; }
    PBEStatus set_face_data(const PBEFaceData* face);
    PBEStatus apply_preset(PBEPreset preset, float strength);
    PBEStatus analyze(const PBEImageView& input, PBESceneStats* out);

    PBEStatus render(const PBEImageView& input, PBEImageView& output);
    PBEStatus auto_enhance();

    const PBEBeautyParams& beauty() const { return beauty_; }
    const std::string& error() const { return last_error_; }

private:
    void set_error(const char* message);

    PBEConfig config_{};
    PBEBeautyParams beauty_{};
    PBEAdvancedColor advanced_color_{};
    PBEAIProvider ai_provider_{};
    bool has_ai_provider_ = false;
    PBEAutoSettings auto_{};
    const PBEFaceData* face_ = nullptr;
    PBEFaceData detected_face_{};
    Pipeline pipeline_;
    std::string last_error_;

    ImageRGBA8 input_rgba_;
    ImageRGBA8 output_rgba_;
};

} // namespace pbe
