#pragma once
#include "pbe/pbe_types.h"
namespace pbe { float sample_curve17(const float curve[17], float x); void apply_curve(float& r,float& g,float& b,const float curve[17]); void reset_advanced_color(PBEAdvancedColor& color); }
