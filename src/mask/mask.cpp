#include "mask/mask.h"
#include <algorithm>
#include <cmath>
namespace pbe::mask {
void Mask::reset(int w,int h,float value){width=w;height=h;data.assign((size_t)w*(size_t)h,value);}
float& Mask::at(int x,int y){return data[(size_t)y*(size_t)width+(size_t)x];}
float Mask::at(int x,int y) const{x=std::max(0,std::min(width-1,x));y=std::max(0,std::min(height-1,y));return data[(size_t)y*(size_t)width+(size_t)x];}
Mask from_face(const float* data,int width,int height){Mask m;m.reset(width,height);if(data) m.data.assign(data,data+(size_t)width*(size_t)height);return m;}
void box_blur(Mask& m,int radius){if(radius<=0||m.data.empty())return; Mask t=m; for(int y=0;y<m.height;y++)for(int x=0;x<m.width;x++){double s=0;int n=0;for(int yy=std::max(0,y-radius);yy<=std::min(m.height-1,y+radius);yy++)for(int xx=std::max(0,x-radius);xx<=std::min(m.width-1,x+radius);xx++){s+=t.at(xx,yy);n++;}m.at(x,y)=float(s/n);}}
void gaussian_blur(Mask& m,int radius){if(radius<=0)return; const float sigma=std::max(0.8f,static_cast<float>(radius)*0.5f); Mask t=m; std::vector<float> k((size_t)radius*2+1);float sum=0;for(int i=-radius;i<=radius;i++){float v=std::exp(-(static_cast<float>(i*i))/(2*sigma*sigma));k[(size_t)(i+radius)]=v;sum+=v;}for(float&v:k)v/=sum; for(int y=0;y<m.height;y++)for(int x=0;x<m.width;x++){float s=0;for(int i=-radius;i<=radius;i++)s+=t.at(x+i,y)*k[(size_t)(i+radius)];m.at(x,y)=s;}t=m;for(int y=0;y<m.height;y++)for(int x=0;x<m.width;x++){float s=0;for(int i=-radius;i<=radius;i++)s+=t.at(x,y+i)*k[(size_t)(i+radius)];m.at(x,y)=s;}}
void dilate(Mask&m,int radius){if(radius<=0)return;Mask t=m;for(int y=0;y<m.height;y++)for(int x=0;x<m.width;x++){float v=0;for(int yy=std::max(0,y-radius);yy<=std::min(m.height-1,y+radius);yy++)for(int xx=std::max(0,x-radius);xx<=std::min(m.width-1,x+radius);xx++)v=std::max(v,t.at(xx,yy));m.at(x,y)=v;}}
void erode(Mask&m,int radius){if(radius<=0)return;Mask t=m;for(int y=0;y<m.height;y++)for(int x=0;x<m.width;x++){float v=1;for(int yy=std::max(0,y-radius);yy<=std::min(m.height-1,y+radius);yy++)for(int xx=std::max(0,x-radius);xx<=std::min(m.width-1,x+radius);xx++)v=std::min(v,t.at(xx,yy));m.at(x,y)=v;}}
void feather(Mask&m,int radius){gaussian_blur(m,radius);clamp01(m);}
void multiply(Mask&dst,const Mask&a,const Mask&b){if(a.width!=b.width||a.height!=b.height)return;dst.reset(a.width,a.height);for(size_t i=0;i<dst.data.size();++i)dst.data[i]=a.data[i]*b.data[i];}
void clamp01(Mask&m){for(float&v:m.data)v=std::max(0.0f,std::min(1.0f,v));}
}
