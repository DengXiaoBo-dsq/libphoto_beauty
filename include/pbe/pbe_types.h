#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum PBEStatus {
    PBE_OK = 0,
    PBE_INVALID_ARGUMENT = -1,
    PBE_INVALID_STATE = -2,
    PBE_OUT_OF_MEMORY = -3,
    PBE_UNSUPPORTED = -4,
    PBE_INTERNAL_ERROR = -5
} PBEStatus;

typedef enum PBEImageFormat {
    PBE_IMAGE_RGBA8 = 0,
    PBE_IMAGE_BGRA8 = 1,
    PBE_IMAGE_RGB8 = 2
} PBEImageFormat;

typedef enum PBERenderQuality {
    PBE_QUALITY_FAST = 0,
    PBE_QUALITY_HIGH = 1,
    PBE_QUALITY_ULTRA = 2
} PBERenderQuality;

typedef enum PBEPreset {
    PBE_PRESET_NEUTRAL = 0,
    PBE_PRESET_NATURAL_PORTRAIT = 1,
    PBE_PRESET_GLOW = 2,
    PBE_PRESET_CINEMATIC = 3,
    PBE_PRESET_FILM = 4,
    PBE_PRESET_CLEAN = 5
} PBEPreset;

typedef struct PBEImageView {
    const uint8_t* data;
    uint8_t* mutable_data;
    int32_t width;
    int32_t height;
    int32_t stride_bytes;
    PBEImageFormat format;
} PBEImageView;

typedef struct PBELandmark {
    float x;
    float y;
    float z;
} PBELandmark;

typedef struct PBEFaceData {
    const PBELandmark* landmarks;
    size_t landmark_count;
    const float* skin_mask;
    const float* person_mask;
    int32_t mask_width;
    int32_t mask_height;
} PBEFaceData;

typedef struct PBESceneStats {
    float mean_luma;
    float contrast;
    float highlight_clip;
    float shadow_clip;
    float warm_score;
    float skin_fraction;
} PBESceneStats;

typedef struct PBEBeautyParams {
    float strength;

    float skin_smooth;
    float skin_tone;
    float skin_bright;
    float blemish_reduction;

    float face_slim;
    float eye_scale;
    float nose_scale;
    float jaw_width;

    float exposure;
    float contrast;
    float highlights;
    float shadows;
    float whites;
    float blacks;
    float temperature;
    float tint;
    float saturation;
    float vibrance;
    float clarity;
    float sharpen;
    float vignette;
    float grain;

    float local_warmth;
    float local_brightness;
    float local_saturation;
} PBEBeautyParams;

typedef struct PBEAutoSettings {
    int32_t enable_auto_exposure;
    int32_t enable_auto_color;
    int32_t enable_auto_beauty;
    int32_t enable_auto_sharpen;
} PBEAutoSettings;

typedef struct PBEConfig {
    PBERenderQuality quality;
    int32_t prefer_gpu;
    int32_t thread_count;
    size_t max_working_memory;
} PBEConfig;

#ifdef __cplusplus
}
#endif
