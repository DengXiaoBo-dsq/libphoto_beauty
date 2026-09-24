#pragma once
#include "core/image.h"
#include "pbe/pbe_geometry.h"
#include "pbe/pbe_types.h"

namespace pbe::beauty {

void apply_face_geometry(
    ImageRGBA8& image,
    const PBEFaceData* face,
    const PBEGeometryParams& params,
    PBERenderQuality quality,
    int threads
);

void apply_body_geometry(
    ImageRGBA8& image,
    const PBEBodyData* body,
    const PBEGeometryParams& params,
    PBERenderQuality quality,
    int threads
);

}
