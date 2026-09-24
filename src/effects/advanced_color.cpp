#include "effects/advanced_color.h"
#include "core/color.h"
#include "core/curves.h"
#include "core/parallel.h"
#include <algorithm>
#include <cmath>
namespace pbe::effects {
static float cd(float a,float b){float d=std::fabs(a-b);return std::min(d,360.f-d);}
static float w(float h,int k){static const float c[8]={0,30,60,150,190,235,285,330};float d=cd(h,c[k]),width=(k==0||k==7)?44.f:38.f;if(d>=width)return 0.f;float t=1.f-d/width;return t*t*(3.f-2.f*t);}
static float ss(float a,float b,float x){float t=std::clamp((x-a)/(b-a),0.f,1.f);return t*t*(3.f-2.f*t);}
void apply_advanced_color(ImageRGBA8&im,const PBEAdvancedColor&c,float master,int threads){master=std::clamp(master,0.f,1.f);if(master<=1e-4f)return;parallel_for_rows(im.height,threads,[&](int ys,int ye){for(int y=ys;y<ye;++y){uint8_t*r=im.row(y);for(int x=0;x<im.width;++x){int i=x*4;RGB q{from_u8(r[i]),from_u8(r[i+1]),from_u8(r[i+2])};apply_curve(q.r,q.g,q.b,c.curve);HSL h=rgb_to_hsl(q);float dh=0,ds=0,dl=0;for(int k=0;k<8;++k){float z=w(h.h,k);dh+=c.hue[k]*z;ds+=c.saturation[k]*z;dl+=c.luminance[k]*z;}h.h+=dh*12.f*master;h.s=std::clamp(h.s*(1.f+ds*.8f*master),0.f,1.f);h.l=std::clamp(h.l+dl*.15f*master,0.f,1.f);q=hsl_to_rgb(h);float lum=luminance(q),sh=1.f-ss(.1f,.55f,lum),hi=ss(.5f,.92f,lum);if(c.color_grading_shadows_s>1e-4f){HSL g=rgb_to_hsl(q);g.h=c.color_grading_shadows_h;g.s=std::clamp(c.color_grading_shadows_s,0.f,1.f);g.l=std::clamp(g.l+c.color_grading_shadows_l*.15f,0.f,1.f);RGB t=hsl_to_rgb(g);float z=sh*c.color_grading_shadows_s*master*.32f;q.r+=(t.r-q.r)*z;q.g+=(t.g-q.g)*z;q.b+=(t.b-q.b)*z;}if(c.color_grading_highlights_s>1e-4f){HSL g=rgb_to_hsl(q);g.h=c.color_grading_highlights_h;g.s=std::clamp(c.color_grading_highlights_s,0.f,1.f);g.l=std::clamp(g.l+c.color_grading_highlights_l*.15f,0.f,1.f);RGB t=hsl_to_rgb(g);float z=hi*c.color_grading_highlights_s*master*.32f;q.r+=(t.r-q.r)*z;q.g+=(t.g-q.g)*z;q.b+=(t.b-q.b)*z;}r[i]=to_u8(q.r);r[i+1]=to_u8(q.g);r[i+2]=to_u8(q.b);}}});}
}
