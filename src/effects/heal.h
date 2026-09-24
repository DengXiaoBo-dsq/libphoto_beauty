#pragma once
#include "core/image.h"
#include "pbe/pbe_geometry.h"
#include <vector>

namespace pbe::effects {
void apply_heal(ImageRGBA8& image, const std::vector<PBEHealPoint>& points, int threads);
}
