#include "core/pipeline.h"
#include "effects/basic.h"
#include "effects/local.h"
#include "beauty/skin.h"
#include "beauty/face_warp.h"
namespace pbe {
Pipeline::Pipeline(){build_graph();}
void Pipeline::build_graph(){
    graph_.clear();
    graph_.add("tone",[](RenderContext& c){effects::apply_basic_tone(*c.image,*c.params);});
    graph_.add("face_warp",[](RenderContext& c){beauty::apply_face_warp(*c.image,c.face,*c.params,c.quality);});
    graph_.add("skin",[](RenderContext& c){beauty::apply_skin(*c.image,c.face,*c.params,c.quality);});
    graph_.add("local",[](RenderContext& c){effects::apply_local_adjustment(*c.image,c.face,*c.params);});
    graph_.add("sharpen",[](RenderContext& c){effects::apply_sharpen(*c.image,c.params->sharpen*c.params->strength);});
    graph_.add("vignette",[](RenderContext& c){if(c.params->vignette>0.001f)effects::apply_vignette(*c.image,c.params->vignette*c.params->strength);});
    graph_.add("grain",[](RenderContext& c){if(c.params->grain>0.001f)effects::apply_grain(*c.image,c.params->grain*c.params->strength);});
}
PBEStatus Pipeline::render(const ImageRGBA8& input,ImageRGBA8& output,const PBEBeautyParams& params,const PBEFaceData* face,PBERenderQuality quality){
    if (input.empty()) return PBE_INVALID_ARGUMENT;
    output = input;
    RenderContext ctx{&output, &params, face, quality};
    graph_.execute(ctx);
    return PBE_OK;
}
}
