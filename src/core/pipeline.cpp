#include "core/pipeline.h"
#include "effects/basic.h"
#include "effects/beauty.h"
#include "effects/mask.h"

namespace pbe {

PBEStatus Pipeline::render(
    const ImageRGBA8& input,
    ImageRGBA8& output,
    const PBEBeautyParams& params,
    const PBEFaceData* face,
    PBERenderQuality quality
) {
    if (input.empty()) return PBE_INVALID_ARGUMENT;

    output = input;

    effects::apply_basic_tone(output, params);
    effects::apply_beauty(output, face, params, quality);
    effects::apply_sharpen(output, params.sharpen * params.strength);

    if (params.vignette > 0.001f) {
        effects::apply_vignette(output, params.vignette * params.strength);
    }
    if (params.grain > 0.001f) {
        effects::apply_grain(output, params.grain * params.strength);
    }

    return PBE_OK;
}

} // namespace pbe
