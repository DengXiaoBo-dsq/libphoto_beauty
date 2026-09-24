#include "core/engine.h"
#include "core/analyzer.h"
#include "core/color.h"
#include <cstring>
#include <cmath>
#include <algorithm>
#include <new>

namespace pbe {
PBEBeautyParams default_beauty_params(){PBEBeautyParams p{};p.strength=1.f;return p;}
PBEAutoSettings default_auto_settings(){return {1,1,1,1};}
PBEBeautyParams preset_params(PBEPreset preset){
    PBEBeautyParams p=default_beauty_params();
    switch(preset){
        case PBE_PRESET_NATURAL_PORTRAIT:p.skin_smooth=.16f;p.skin_tone=.05f;p.skin_bright=.03f;p.sharpen=.08f;p.vibrance=.04f;p.exposure=.04f;break;
        case PBE_PRESET_GLOW:p.skin_smooth=.12f;p.skin_bright=.07f;p.exposure=.08f;p.highlights=-.08f;p.shadows=.10f;p.temperature=.03f;p.vibrance=.05f;break;
        case PBE_PRESET_CINEMATIC:p.contrast=.10f;p.highlights=-.18f;p.shadows=.12f;p.blacks=-.04f;p.temperature=-.03f;p.saturation=-.03f;p.vibrance=.08f;p.grain=.08f;p.vignette=.10f;break;
        case PBE_PRESET_FILM:p.contrast=.06f;p.highlights=-.12f;p.shadows=.08f;p.temperature=.02f;p.saturation=-.06f;p.vibrance=.06f;p.grain=.14f;p.vignette=.05f;break;
        case PBE_PRESET_CLEAN:p.contrast=.03f;p.highlights=-.10f;p.shadows=.06f;p.vibrance=.04f;p.sharpen=.12f;break;
        default:break;
    }
    return p;
}
Engine::Engine(const PBEConfig& cfg):config_(cfg),beauty_(default_beauty_params()),auto_(default_auto_settings()){}
void Engine::set_error(const char* m){last_error_=m?m:"";}
PBEStatus Engine::reset(){beauty_=default_beauty_params();auto_=default_auto_settings();face_=nullptr;input_rgba_.pixels.clear();output_rgba_.pixels.clear();last_error_.clear();return PBE_OK;}
PBEStatus Engine::set_beauty(const PBEBeautyParams&p){beauty_=p;return PBE_OK;}
PBEStatus Engine::set_auto(const PBEAutoSettings&s){auto_=s;return PBE_OK;}
PBEStatus Engine::set_face_data(const PBEFaceData*f){face_=f;return PBE_OK;}
PBEStatus Engine::apply_preset(PBEPreset preset,float strength){beauty_=preset_params(preset);beauty_.strength=std::clamp(strength,0.f,1.f);return PBE_OK;}
static PBEStatus import_image(const PBEImageView&in,ImageRGBA8&dst){if(!in.data||in.width<=0||in.height<=0||in.stride_bytes<=0)return PBE_INVALID_ARGUMENT;dst.reset(in.width,in.height);for(int y=0;y<in.height;y++){const uint8_t*s=in.data+(size_t)y*(size_t)in.stride_bytes;uint8_t*d=dst.row(y);if(in.format==PBE_IMAGE_RGBA8)std::memcpy(d,s,(size_t)in.width*4u);else if(in.format==PBE_IMAGE_BGRA8)for(int x=0;x<in.width;x++){d[x*4]=s[x*4+2];d[x*4+1]=s[x*4+1];d[x*4+2]=s[x*4];d[x*4+3]=s[x*4+3];}else if(in.format==PBE_IMAGE_RGB8)for(int x=0;x<in.width;x++){d[x*4]=s[x*3];d[x*4+1]=s[x*3+1];d[x*4+2]=s[x*3+2];d[x*4+3]=255;}else return PBE_UNSUPPORTED;}return PBE_OK;}
static PBEStatus export_image(const ImageRGBA8&src,PBEImageView&out){if(!out.mutable_data||out.width!=src.width||out.height!=src.height||out.stride_bytes<=0)return PBE_INVALID_ARGUMENT;for(int y=0;y<src.height;y++){const uint8_t*s=src.row(y);uint8_t*d=out.mutable_data+(size_t)y*(size_t)out.stride_bytes;if(out.format==PBE_IMAGE_RGBA8)std::memcpy(d,s,(size_t)src.width*4u);else if(out.format==PBE_IMAGE_BGRA8)for(int x=0;x<src.width;x++){d[x*4]=s[x*4+2];d[x*4+1]=s[x*4+1];d[x*4+2]=s[x*4];d[x*4+3]=s[x*4+3];}else return PBE_UNSUPPORTED;}return PBE_OK;}
PBEStatus Engine::render(const PBEImageView&input,PBEImageView&output){if(import_image(input,input_rgba_)!=PBE_OK){set_error("Unsupported input image.");return PBE_INVALID_ARGUMENT;}PBEStatus s=pipeline_.render(input_rgba_,output_rgba_,beauty_,face_,config_.quality);if(s!=PBE_OK){set_error("Pipeline render failed.");return s;}return export_image(output_rgba_,output);}
PBEStatus Engine::analyze(const PBEImageView&input,PBESceneStats*out){if(!out)return PBE_INVALID_ARGUMENT;PBEStatus s=import_image(input,input_rgba_);if(s!=PBE_OK)return s;*out=Analyzer{}.analyze(input_rgba_);return PBE_OK;}
PBEStatus Engine::auto_enhance(){if(input_rgba_.empty()){set_error("Analyze or render an image before auto_enhance.");return PBE_INVALID_STATE;}PBESceneStats s=Analyzer{}.analyze(input_rgba_);if(auto_.enable_auto_exposure){beauty_.exposure+=std::clamp((.48f-s.mean_luma)*1.35f,-1.0f,1.0f);}if(auto_.enable_auto_color){beauty_.contrast+=std::clamp((.21f-s.contrast)*.7f,-.12f,.12f);beauty_.vibrance+=.04f*(1.f-std::min(1.f,s.skin_fraction));if(s.highlight_clip>.02f)beauty_.highlights-=.12f;if(s.shadow_clip>.03f)beauty_.shadows+=.10f;}if(auto_.enable_auto_beauty&&s.skin_fraction>.02f){beauty_.skin_smooth=std::max(beauty_.skin_smooth,.12f);beauty_.skin_bright=std::max(beauty_.skin_bright,.03f);}if(auto_.enable_auto_sharpen){beauty_.sharpen=std::max(beauty_.sharpen,.08f);}return PBE_OK;}
}
