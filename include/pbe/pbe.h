#pragma once
#include "pbe_types.h"
#include "pbe_geometry.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef void* PBEHandle;

PBEStatus pbe_create(const PBEConfig* config, PBEHandle* out_handle);
void pbe_destroy(PBEHandle handle);
PBEStatus pbe_reset(PBEHandle handle);

PBEStatus pbe_set_beauty_params(PBEHandle handle, const PBEBeautyParams* params);
PBEStatus pbe_set_advanced_color(PBEHandle handle, const PBEAdvancedColor* params);
PBEStatus pbe_get_advanced_color(PBEHandle handle, PBEAdvancedColor* out_params);
PBEStatus pbe_set_ai_provider(PBEHandle handle, const PBEAIProvider* provider);
PBEStatus pbe_get_beauty_params(PBEHandle handle, PBEBeautyParams* out_params);
PBEStatus pbe_apply_preset(PBEHandle handle, PBEPreset preset, float strength);

PBEStatus pbe_set_auto_settings(PBEHandle handle, const PBEAutoSettings* settings);
PBEStatus pbe_set_face_data(PBEHandle handle, const PBEFaceData* face_data);

PBEStatus pbe_analyze_rgba8(PBEHandle handle, const PBEImageView* input, PBESceneStats* out_stats);
PBEStatus pbe_auto_enhance(PBEHandle handle);

PBEStatus pbe_render_rgba8(PBEHandle handle, const PBEImageView* input, PBEImageView* output);
const char* pbe_get_last_error(PBEHandle handle);

#ifdef __cplusplus
}
#endif
