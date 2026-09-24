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

typedef enum PBEHSLChannel {
    PBE_HSL_RED = 0, PBE_HSL_ORANGE = 1, PBE_HSL_YELLOW = 2, PBE_HSL_GREEN = 3,
    PBE_HSL_AQUA = 4, PBE_HSL_BLUE = 5, PBE_HSL_PURPLE = 6, PBE_HSL_MAGENTA = 7,
    PBE_HSL_CHANNEL_COUNT = 8
} PBEHSLChannel;

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

typedef struct PBEAdvancedColor {
    float curve[17];
    float hue[PBE_HSL_CHANNEL_COUNT];
    float saturation[PBE_HSL_CHANNEL_COUNT];
    float luminance[PBE_HSL_CHANNEL_COUNT];
    float color_grading_shadows_h;
    float color_grading_shadows_s;
    float color_grading_shadows_l;
    float color_grading_highlights_h;
    float color_grading_highlights_s;
    float color_grading_highlights_l;
    float color_grading_balance;
} PBEAdvancedColor;

typedef struct PBEAIProvider {
    void* user_data;
    int (*analyze)(void* user_data, const uint8_t* rgba, int32_t width, int32_t height, PBEFaceData* out_face);
} PBEAIProvider;

typedef struct PBEConfig {
    PBERenderQuality quality;
    int32_t prefer_gpu;
    int32_t thread_count;
    size_t max_working_memory;
} PBEConfig;

#ifdef __cplusplus
}
#endif
