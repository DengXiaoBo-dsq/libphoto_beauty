#pragma once

#include "pbe/pbe_types.h"
#include "core/image.h"

namespace pbe {

class Pipeline {
public:
    PBEStatus render(
        const ImageRGBA8& input,
        ImageRGBA8& output,
        const PBEBeautyParams& params,
        const PBEFaceData* face,
        PBERenderQuality quality
    );
};

} // namespace pbe
