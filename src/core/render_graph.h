#pragma once
#include "core/image.h"
#include "pbe/pbe_types.h"
#include <string>
#include <vector>
#include <functional>

namespace pbe {
struct RenderContext {
    ImageRGBA8* image = nullptr;
    const PBEBeautyParams* params = nullptr;
    const PBEFaceData* face = nullptr;
    PBERenderQuality quality = PBE_QUALITY_HIGH;
};

class RenderGraph {
public:
    using Pass = std::function<void(RenderContext&)>;
    void clear();
    void add(const char* name, Pass pass);
    void execute(RenderContext& ctx) const;
    size_t size() const { return passes_.size(); }
private:
    struct Node { std::string name; Pass fn; };
    std::vector<Node> passes_;
};
}
