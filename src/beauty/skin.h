#pragma once
#include "core/image.h"
#include "pbe/pbe_types.h"
namespace pbe::beauty {
void apply_skin(ImageRGBA8& image,const PBEFaceData* face,const PBEBeautyParams& params,PBERenderQuality quality);
}
