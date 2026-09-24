#pragma once

#include "core/image.h"

namespace pbe {

class GPUBackend {
public:
    virtual ~GPUBackend() = default;
    virtual bool available() const = 0;
    virtual bool process(ImageRGBA8& image) = 0;
};

} // namespace pbe
