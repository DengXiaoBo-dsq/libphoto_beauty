#include "effects/heal.h"
#include "core/parallel.h"
#include <algorithm>
#include <cmath>

namespace pbe::effects {
namespace {
uint8_t bilerp(const ImageRGBA8& im, float x, float y, int c) {
    x = std::clamp(x, 0.0f, static_cast<float>(im.width - 1));
    y = std::clamp(y, 0.0f, static_cast<float>(im.height - 1));
    const int x0 = static_cast<int>(x);
    const int y0 = static_cast<int>(y);
    const int x1 = std::min(im.width-1,x0+1);
    const int y1 = std::min(im.height-1,y0+1);
    const float fx=x-static_cast<float>(x0), fy=y-static_cast<float>(y0);
    const float a=im.row(y0)[x0*4+c], b=im.row(y0)[x1*4+c];
    const float d=im.row(y1)[x0*4+c], e=im.row(y1)[x1*4+c];
    const float ab=a+(b-a)*fx, de=d+(e-d)*fx;
    return static_cast<uint8_t>(std::clamp(ab+(de-ab)*fy,0.0f,255.0f));
}
float w_smooth(float d,float r){
    if(r<=1e-6f||d>=r)return 0.0f;
    const float t=1.0f-d/r;
    return t*t*(3.0f-2.0f*t);
}
}
void apply_heal(ImageRGBA8& image, const std::vector<PBEHealPoint>& points, int threads){
    if(points.empty()||image.empty()) return;
    const ImageRGBA8 src=image;
    parallel_for_rows(image.height, threads, [&](int ys,int ye){
        for(int y=ys;y<ye;++y){
            uint8_t* row=image.row(y);
            for(int x=0;x<image.width;++x){
                float out_r=src.row(y)[x*4+0], out_g=src.row(y)[x*4+1], out_b=src.row(y)[x*4+2];
                float total=0.0f;
                const float u=float(x)/float(std::max(1,image.width-1));
                const float v=float(y)/float(std::max(1,image.height-1));
                for(const auto& pt:points){
                    const float r=std::clamp(pt.radius,0.001f,0.20f);
                    const float d=std::hypot(u-pt.x,v-pt.y);
                    const float w=w_smooth(d,r)*std::clamp(pt.strength,0.0f,1.0f);
                    if(w<=0.0f)continue;
                    // Choose an offset outside the defect and copy texture/color back toward center.
                    const float dx=u-pt.x, dy=v-pt.y;
                    const float len=std::max(1e-4f,std::hypot(dx,dy));
                    const float ox=pt.x+dx/len*r*1.15f;
                    const float oy=pt.y+dy/len*r*1.15f;
                    const float sr=bilerp(src,ox*static_cast<float>(src.width-1),oy*static_cast<float>(src.height-1),0);
                    const float sg=bilerp(src,ox*static_cast<float>(src.width-1),oy*static_cast<float>(src.height-1),1);
                    const float sb=bilerp(src,ox*static_cast<float>(src.width-1),oy*static_cast<float>(src.height-1),2);
                    out_r=out_r*(1.0f-w)+sr*w;
                    out_g=out_g*(1.0f-w)+sg*w;
                    out_b=out_b*(1.0f-w)+sb*w;
                    total=std::min(1.0f,total+w);
                }
                row[x*4+0]=static_cast<uint8_t>(std::clamp(out_r,0.0f,255.0f));
                row[x*4+1]=static_cast<uint8_t>(std::clamp(out_g,0.0f,255.0f));
                row[x*4+2]=static_cast<uint8_t>(std::clamp(out_b,0.0f,255.0f));
                (void)total;
            }
        }
    });
}
}
