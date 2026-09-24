#include "core/pipeline.h"
#include "effects/basic.h"
#include "effects/local.h"
#include "beauty/skin.h"
#include "beauty/face_warp.h"
#include "effects/advanced_color.h"
#include "effects/heal.h"
#include "beauty/geometry.h"
namespace pbe {
Pipeline::Pipeline(){build_graph();}
void Pipeline::build_graph(){
    graph_.clear();
    graph_.add("tone",[](RenderContext& c){effects::apply_basic_tone(*c.image,*c.params);});
    graph_.add("advanced_color",[](RenderContext& c){if(c.advanced_color)effects::apply_advanced_color(*c.image,*c.advanced_color,c.params->strength,c.threads);});
    graph_.add("face_geometry",[](RenderContext& c){if(c.geometry)beauty::apply_face_geometry(*c.image,c.face,*c.geometry,c.quality,c.threads);});
    graph_.add("body_geometry",[](RenderContext& c){if(c.geometry)beauty::apply_body_geometry(*c.image,c.body,*c.geometry,c.quality,c.threads);});
    graph_.add("skin",[](RenderContext& c){beauty::apply_skin(*c.image,c.face,*c.params,c.quality);});
    graph_.add("local",[](RenderContext& c){effects::apply_local_adjustment(*c.image,c.face,*c.params);});
    graph_.add("sharpen",[](RenderContext& c){effects::apply_sharpen(*c.image,c.params->sharpen*c.params->strength);});
    graph_.add("vignette",[](RenderContext& c){if(c.params->vignette>0.001f)effects::apply_vignette(*c.image,c.params->vignette*c.params->strength);});
    graph_.add("heal",[](RenderContext& c){if(c.heal_points && !c.heal_points->empty())effects::apply_heal(*c.image,*c.heal_points,c.threads);});
    graph_.add("grain",[](RenderContext& c){if(c.params->grain>0.001f)effects::apply_grain(*c.image,c.params->grain*c.params->strength);});
}
PBEStatus Pipeline::render(const ImageRGBA8& input,ImageRGBA8& output,const PBEBeautyParams& params,const PBEFaceData* face,const PBEAdvancedColor& color,const PBEGeometryParams& geometry,const PBEBodyData* body,const std::vector<PBEHealPoint>& heal_points,PBERenderQuality quality,int threads){
    if (input.empty()) return PBE_INVALID_ARGUMENT;
    output = input;
    RenderContext ctx{&output, &params, face, &color, &geometry, body, &heal_points, threads, quality};
    graph_.execute(ctx);
    return PBE_OK;
}
}
