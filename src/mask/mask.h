#pragma once
#include <vector>
#include <cstdint>
namespace pbe::mask {
struct Mask {
    int width=0, height=0;
    std::vector<float> data;
    void reset(int w,int h,float value=0.0f);
    float& at(int x,int y);
    float at(int x,int y) const;
};
Mask from_face(const float* data,int width,int height);
void gaussian_blur(Mask& m, int radius);
void box_blur(Mask& m, int radius);
void dilate(Mask& m, int radius);
void erode(Mask& m, int radius);
void feather(Mask& m, int radius);
void multiply(Mask& dst,const Mask& a,const Mask& b);
void clamp01(Mask& m);
}
