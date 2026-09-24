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

typedef struct PBEImageView {
    const uint8_t* data;
    uint8_t* mutable_data;
    int32_t width;
    int32_t height;
    int32_t stride_bytes;
    PBEImageFormat format;
} PBEImageView;

typedef struct PBELandmark {
    float x; // normalized [0,1]
    float y; // normalized [0,1]
    float z;
} PBELandmark;

typedef struct PBEFaceData {
    const PBELandmark* landmarks;
    size_t landmark_count;
    const float* skin_mask;     // width*height, [0,1], optional
    const float* person_mask;   // width*height, [0,1], optional
    int32_t mask_width;
    int32_t mask_height;
} PBEFaceData;

typedef struct PBEBeautyParams {
    float strength;             // 0..1 master strength

    float skin_smooth;          // 0..1
    float skin_tone;            // -1..1
    float skin_bright;          // -1..1
    float blemish_reduction;    // 0..1

    float face_slim;            // -1..1
    float eye_scale;            // -1..1
    float nose_scale;           // -1..1
    float jaw_width;            // -1..1

    float exposure;             // EV-ish [-4,4]
    float contrast;             // [-1,1]
    float highlights;           // [-1,1]
    float shadows;              // [-1,1]
    float temperature;          // [-1,1]
    float tint;                 // [-1,1]
    float saturation;            // [-1,1]
    float vibrance;              // [-1,1]
    float clarity;               // [-1,1]
    float sharpen;               // [0,1]
    float vignette;              // [0,1]

    float grain;                 // [0,1]
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
    int32_t thread_count;        // 0 = auto
    size_t max_working_memory;   // 0 = engine default
} PBEConfig;

#ifdef __cplusplus
}
#endif
