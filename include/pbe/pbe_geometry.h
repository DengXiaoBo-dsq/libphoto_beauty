#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum PBEFacePointIndex {
    PBE_FACE_LEFT_EYE = 0,
    PBE_FACE_RIGHT_EYE = 1,
    PBE_FACE_NOSE = 2,
    PBE_FACE_MOUTH = 3,
    PBE_FACE_CHIN = 4
} PBEFacePointIndex;

typedef struct PBEGeometryParams {
    float face_slim;          /* -1..1 */
    float jaw_width;          /* -1..1 */
    float chin_length;        /* -1..1 */
    float eye_scale;          /* -1..1 */
    float eye_vertical;       /* -1..1 */
    float eye_spacing;        /* -1..1 */
    float nose_scale;         /* -1..1 */
    float nose_bridge;        /* -1..1 */
    float mouth_scale;        /* -1..1 */
    float mouth_vertical;     /* -1..1 */

    float body_slim;          /* -1..1 */
    float waist;              /* -1..1 */
    float shoulders;          /* -1..1 */
    float arms;               /* -1..1 */
    float legs;               /* -1..1 */
    float height;             /* -1..1 */
} PBEGeometryParams;

typedef struct PBEPosePoint {
    float x;                  /* normalized */
    float y;                  /* normalized */
    float confidence;         /* 0..1 */
} PBEPosePoint;

typedef struct PBEBodyData {
    const PBEPosePoint* points;
    size_t point_count;
    const float* person_mask;
    int32_t mask_width;
    int32_t mask_height;
} PBEBodyData;

typedef struct PBEHealPoint {
    float x;                  /* normalized */
    float y;                  /* normalized */
    float radius;             /* normalized */
    float strength;           /* 0..1 */
} PBEHealPoint;

#ifdef __cplusplus
}
#endif
