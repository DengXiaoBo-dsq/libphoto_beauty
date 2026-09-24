#include "effects/local.h"
#include "effects/mask.h"
#include "core/color.h"
#include <algorithm>
#include <cmath>
namespace pbe::effects {
void apply_local_adjustment(ImageRGBA8& image,const PBEFaceData* face,const PBEBeautyParams& params){
    if (!face) return;
    const bool hasPerson = face->person_mask && face->mask_width == image.width && face->mask_height == image.height;
    if (!hasPerson) return;
    const float wb=params.local_warmth*params.strength, br=params.local_brightness*params.strength, sat=params.local_saturation*params.strength;if(std::fabs(wb)+std::fabs(br)+std::fabs(sat)<1e-4f)return;
    for(int y=0;y<image.height;y++)for(int x=0;x<image.width;x++){
        int i=x*4;float m=sample_mask(face->person_mask,image.width,image.height,x,y);if(m<.001f)continue;RGB c{image.row(y)[i]/255.f,image.row(y)[i+1]/255.f,image.row(y)[i+2]/255.f};HSL h=rgb_to_hsl(c);h.l=std::clamp(h.l+br*.08f*m,0.f,1.f);h.s=std::clamp(h.s*(1.f+sat*.5f*m),0.f,1.f);h.h+=wb*4.f*m;c=hsl_to_rgb(h);image.row(y)[i]=to_u8(c.r);image.row(y)[i+1]=to_u8(c.g);image.row(y)[i+2]=to_u8(c.b);
    }
}
}
