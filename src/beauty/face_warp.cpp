#include "beauty/face_warp.h"
#include <algorithm>
#include <cmath>
namespace pbe::beauty {
static PBELandmark lm(const PBEFaceData* f, size_t idx, PBELandmark fallback){
    if (f && f->landmarks && f->landmark_count > idx) return f->landmarks[idx];
    return fallback;
}
static uint8_t bilerp(const ImageRGBA8& im,float x,float y,int c){
    x=std::max(0.f,std::min(float(im.width-1),x));y=std::max(0.f,std::min(float(im.height-1),y));
    int x0=int(x),y0=int(y),x1=std::min(im.width-1,x0+1),y1=std::min(im.height-1,y0+1);float fx=x-static_cast<float>(x0),fy=y-static_cast<float>(y0);
    const float a=im.row(y0)[x0*4+c],b=im.row(y0)[x1*4+c],d=im.row(y1)[x0*4+c],e=im.row(y1)[x1*4+c];
    return uint8_t(std::clamp((a+(b-a)*fx)+((d+(e-d)*fx)-(a+(b-a)*fx))*fy,0.f,255.f));
}
static float radial_inv(float dx,float dy,float strength,float radius){
    float r=std::sqrt(dx*dx+dy*dy);if(r>=radius||radius<=1e-4f)return 0.f;float t=1.f-r/radius;return strength*t*t;
}
void apply_face_warp(ImageRGBA8& image,const PBEFaceData* face,const PBEBeautyParams& params,PBERenderQuality quality){
    if(!face||!face->landmarks||face->landmark_count<5)return;
    const size_t li=(face->landmark_count>=468)?33:0;
    const size_t ri=(face->landmark_count>=468)?263:1;
    const size_t ni=(face->landmark_count>=468)?1:2;
    const size_t mi=(face->landmark_count>=468)?13:3;
    PBELandmark L=lm(face,li,{0.35f,0.45f,0}),R=lm(face,ri,{0.65f,0.45f,0}),N=lm(face,ni,{0.50f,0.52f,0}),M=lm(face,mi,{0.50f,0.68f,0});
    const float cx=(L.x+R.x)*0.5f, cy=(L.y+R.y)*0.5f;
    const float faceR=std::max(0.16f,std::min(0.48f,std::hypot(R.x-L.x,R.y-L.y)*1.55f));
    const float slim=std::clamp(params.face_slim*params.strength,-0.45f,0.45f);
    const float jaw=std::clamp(params.jaw_width*params.strength,-0.35f,0.35f);
    const float eye=std::clamp(params.eye_scale*params.strength,-0.25f,0.25f);
    if(std::fabs(slim)+std::fabs(jaw)+std::fabs(eye)<1e-4f)return;
    ImageRGBA8 src=image;
    for(int y=0;y<image.height;y++)for(int x=0;x<image.width;x++){
        float u=static_cast<float>(x)/static_cast<float>(std::max(1,image.width-1)),v=static_cast<float>(y)/static_cast<float>(std::max(1,image.height-1));
        float sx=u,sy=v;
        float dx=u-cx,dy=v-cy;
        float s=radial_inv(dx,dy,slim*0.55f,faceR);
        float j=radial_inv(dx,dy,jaw*0.40f,faceR*0.95f);
        sx += dx*(s+j*0.45f);
        // Eyes expand locally around each eye center.
        for(PBELandmark E:{L,R}){
            float ex=u-E.x,ey=v-E.y;float e=radial_inv(ex,ey,eye,0.11f);
            sx += ex*e; sy += ey*e;
        }
        // Mild nose refinement: scale around nose only, deliberately constrained.
        float nx=u-N.x,ny=v-N.y;float ns=radial_inv(nx,ny,params.nose_scale*params.strength*0.12f,0.09f);sx+=nx*ns;sy+=ny*ns;
        if(sx!=u||sy!=v){int ix=int(sx*float(src.width-1)),iy=int(sy*float(src.height-1));(void)ix;(void)iy;}
        for(int c=0;c<4;c++) image.row(y)[x*4+c]=(c==3)?src.row(y)[x*4+c]:bilerp(src,sx*float(src.width-1),sy*float(src.height-1),c);
    }
    (void)quality;
    (void)M;
}
}
