#pragma once
#include "pbe/pbe_types.h"
#include "pbe/pbe_geometry.h"
#include "core/image.h"
#include "core/render_graph.h"
namespace pbe {
class Pipeline {
public:
    Pipeline();
    PBEStatus render(const ImageRGBA8& input,ImageRGBA8& output,const PBEBeautyParams& params,const PBEFaceData* face,const PBEAdvancedColor& color,const PBEGeometryParams& geometry,const PBEBodyData* body,const std::vector<PBEHealPoint>& heal_points,PBERenderQuality quality,int threads);
private:
    RenderGraph graph_;
    void build_graph();
};
}
