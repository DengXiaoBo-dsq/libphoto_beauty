#pragma once

#include "core/image.h"
#include "pbe/pbe_types.h"

namespace pbe::effects {

void apply_basic_tone(ImageRGBA8& image, const PBEBeautyParams& params);
void apply_sharpen(ImageRGBA8& image, float amount);
void apply_vignette(ImageRGBA8& image, float amount);
void apply_grain(ImageRGBA8& image, float amount);

} // namespace pbe::effects
