#pragma once

#include "pbe/pbe_types.h"

namespace pbe::effects {

float sample_mask(const float* mask, int width, int height, int x, int y);

bool fallback_skin_probability(float r, float g, float b);

} // namespace pbe::effects
