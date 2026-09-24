#pragma once
#include "core/image.h"
#include "pbe/pbe_types.h"
namespace pbe {
class Analyzer {
public:
    PBESceneStats analyze(const ImageRGBA8& image) const;
};
}
