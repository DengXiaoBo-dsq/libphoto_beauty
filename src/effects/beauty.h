#pragma once

#include "core/image.h"
#include "pbe/pbe_types.h"

namespace pbe::effects {

void apply_beauty(
    ImageRGBA8& image,
    const PBEFaceData* face,
    const PBEBeautyParams& params,
    PBERenderQuality quality
);

} // namespace pbe::effects
