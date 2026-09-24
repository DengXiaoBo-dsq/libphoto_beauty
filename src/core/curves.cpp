#include "core/curves.h"
#include <algorithm>
namespace pbe {
float sample_curve17(const float c[17],float x){x=std::clamp(x,0.f,1.f);float p=x*16.f;int i=std::min(15,(int)p);float t=p-(float)i;return std::clamp(c[i]+(c[i+1]-c[i])*t,0.f,1.f);}
void apply_curve(float&r,float&g,float&b,const float c[17]){r=sample_curve17(c,r);g=sample_curve17(c,g);b=sample_curve17(c,b);}
void reset_advanced_color(PBEAdvancedColor& c){for(int i=0;i<17;++i)c.curve[i]=(float)i/16.f;for(int i=0;i<8;++i)c.hue[i]=c.saturation[i]=c.luminance[i]=0.f;c.color_grading_shadows_h=c.color_grading_shadows_s=c.color_grading_shadows_l=0.f;c.color_grading_highlights_h=c.color_grading_highlights_s=c.color_grading_highlights_l=0.f;c.color_grading_balance=0.f;}
}
