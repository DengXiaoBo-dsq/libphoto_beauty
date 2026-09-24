#pragma once

#include "pbe/pbe_types.h"

namespace pbe {

class AIBackend {
public:
    virtual ~AIBackend() = default;

    virtual bool available() const = 0;
    virtual bool analyze_face(
        const uint8_t* rgba,
        int width,
        int height,
        PBEFaceData& out
    ) = 0;
};

} // namespace pbe
