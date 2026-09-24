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
    PBEStatus set_face_data(const PBEFaceData* face);

    PBEStatus render(const PBEImageView& input, PBEImageView& output);
    PBEStatus auto_enhance();

    const PBEBeautyParams& beauty() const { return beauty_; }
    const std::string& error() const { return last_error_; }

private:
    void set_error(const char* message);

    PBEConfig config_{};
    PBEBeautyParams beauty_{};
    PBEAutoSettings auto_{};
    const PBEFaceData* face_ = nullptr;
    Pipeline pipeline_;
    std::string last_error_;

    ImageRGBA8 input_rgba_;
    ImageRGBA8 output_rgba_;
};

} // namespace pbe
