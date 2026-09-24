#include "pbe/pbe.h"
#include "core/engine.h"
#include <new>
extern "C" {
PBEStatus pbe_create(const PBEConfig* config,PBEHandle*out_handle){if(!out_handle)return PBE_INVALID_ARGUMENT;PBEConfig cfg{};cfg.quality=PBE_QUALITY_HIGH;cfg.prefer_gpu=1;cfg.thread_count=0;cfg.max_working_memory=0;if(config)cfg=*config;try{auto*e=new pbe::Engine(cfg);*out_handle=reinterpret_cast<PBEHandle>(e);return PBE_OK;}catch(const std::bad_alloc&){return PBE_OUT_OF_MEMORY;}catch(...){return PBE_INTERNAL_ERROR;}}
void pbe_destroy(PBEHandle h){delete reinterpret_cast<pbe::Engine*>(h);}
PBEStatus pbe_reset(PBEHandle h){return h?reinterpret_cast<pbe::Engine*>(h)->reset():PBE_INVALID_ARGUMENT;}
PBEStatus pbe_set_beauty_params(PBEHandle h,const PBEBeautyParams*p){return(!h||!p)?PBE_INVALID_ARGUMENT:reinterpret_cast<pbe::Engine*>(h)->set_beauty(*p);}
PBEStatus pbe_set_advanced_color(PBEHandle h,const PBEAdvancedColor*p){return(!h||!p)?PBE_INVALID_ARGUMENT:reinterpret_cast<pbe::Engine*>(h)->set_advanced_color(*p);}
PBEStatus pbe_get_advanced_color(PBEHandle h,PBEAdvancedColor*out){if(!h||!out)return PBE_INVALID_ARGUMENT;*out=reinterpret_cast<pbe::Engine*>(h)->advanced_color();return PBE_OK;}
PBEStatus pbe_set_ai_provider(PBEHandle h,const PBEAIProvider*p){return h?reinterpret_cast<pbe::Engine*>(h)->set_ai_provider(p):PBE_INVALID_ARGUMENT;}
PBEStatus pbe_get_beauty_params(PBEHandle h,PBEBeautyParams*out){if(!h||!out)return PBE_INVALID_ARGUMENT;*out=reinterpret_cast<pbe::Engine*>(h)->beauty();return PBE_OK;}
PBEStatus pbe_apply_preset(PBEHandle h,PBEPreset preset,float strength){return h?reinterpret_cast<pbe::Engine*>(h)->apply_preset(preset,strength):PBE_INVALID_ARGUMENT;}
PBEStatus pbe_set_auto_settings(PBEHandle h,const PBEAutoSettings*s){return(!h||!s)?PBE_INVALID_ARGUMENT:reinterpret_cast<pbe::Engine*>(h)->set_auto(*s);}
PBEStatus pbe_set_face_data(PBEHandle h,const PBEFaceData*f){return h?reinterpret_cast<pbe::Engine*>(h)->set_face_data(f):PBE_INVALID_ARGUMENT;}
PBEStatus pbe_analyze_rgba8(PBEHandle h,const PBEImageView*i,PBESceneStats*out){return(!h||!i||!out)?PBE_INVALID_ARGUMENT:reinterpret_cast<pbe::Engine*>(h)->analyze(*i,out);}
PBEStatus pbe_auto_enhance(PBEHandle h){return h?reinterpret_cast<pbe::Engine*>(h)->auto_enhance():PBE_INVALID_ARGUMENT;}
PBEStatus pbe_render_rgba8(PBEHandle h,const PBEImageView*i,PBEImageView*o){return(!h||!i||!o)?PBE_INVALID_ARGUMENT:reinterpret_cast<pbe::Engine*>(h)->render(*i,*o);}
const char*pbe_get_last_error(PBEHandle h){return h?reinterpret_cast<pbe::Engine*>(h)->error().c_str():"Invalid handle.";}
}
