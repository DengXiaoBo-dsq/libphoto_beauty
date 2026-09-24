#include "beauty/geometry.h"
#include "effects/mask.h"
#include "core/parallel.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace pbe::beauty {

namespace {

PBELandmark point_or(const PBEFaceData* f, size_t i, PBELandmark fallback) {
    if (f && f->landmarks && i < f->landmark_count) return f->landmarks[i];
    return fallback;
}

float falloff(float d, float radius) {
    if (radius <= 1e-6f || d >= radius) return 0.0f;
    const float t = 1.0f - d / radius;
    return t * t * (3.0f - 2.0f * t);
}

float safe_float(float v) {
    return std::isfinite(v) ? v : 0.0f;
}

uint8_t bilerp(const ImageRGBA8& im, float x, float y, int c) {
    x = std::clamp(x, 0.0f, static_cast<float>(im.width - 1));
    y = std::clamp(y, 0.0f, static_cast<float>(im.height - 1));
    const int x0 = static_cast<int>(x);
    const int y0 = static_cast<int>(y);
    const int x1 = std::min(im.width - 1, x0 + 1);
    const int y1 = std::min(im.height - 1, y0 + 1);
    const float fx = x - static_cast<float>(x0);
    const float fy = y - static_cast<float>(y0);

    const float a = static_cast<float>(im.row(y0)[x0 * 4 + c]);
    const float b = static_cast<float>(im.row(y0)[x1 * 4 + c]);
    const float d = static_cast<float>(im.row(y1)[x0 * 4 + c]);
    const float e = static_cast<float>(im.row(y1)[x1 * 4 + c]);
    const float ab = a + (b - a) * fx;
    const float de = d + (e - d) * fx;
    return static_cast<uint8_t>(std::clamp(ab + (de - ab) * fy, 0.0f, 255.0f));
}

void local_scale(float u, float v, float cx, float cy, float radius,
                 float amount, float& sx, float& sy) {
    const float dx = u - cx;
    const float dy = v - cy;
    const float d = std::hypot(dx, dy);
    const float w = falloff(d, radius) * amount;
    // Inverse mapping: positive amount makes the edited feature appear larger.
    sx += dx * w;
    sy += dy * w;
}

float person_weight(const PBEBodyData* body, float u, float v) {
    if (!body || !body->person_mask || body->mask_width <= 1 || body->mask_height <= 1) return 1.0f;
    const int x = std::clamp(static_cast<int>(u * static_cast<float>(body->mask_width)), 0, body->mask_width - 1);
    const int y = std::clamp(static_cast<int>(v * static_cast<float>(body->mask_height)), 0, body->mask_height - 1);
    return std::clamp(body->person_mask[y * body->mask_width + x], 0.0f, 1.0f);
}

PBEPosePoint pose(const PBEBodyData* body, size_t i) {
    if (!body || !body->points || i >= body->point_count) return {0.5f, 0.5f, 0.0f};
    return body->points[i];
}

} // namespace

void apply_face_geometry(ImageRGBA8& image, const PBEFaceData* face,
                         const PBEGeometryParams& p, PBERenderQuality quality, int threads) {
    if (!face || !face->landmarks || face->landmark_count < 5) return;

    const size_t li = face->landmark_count >= 468 ? 33 : 0;
    const size_t ri = face->landmark_count >= 468 ? 263 : 1;
    const size_t ni = face->landmark_count >= 468 ? 1 : 2;
    const size_t mi = face->landmark_count >= 468 ? 13 : 3;
    const size_t ci = face->landmark_count >= 468 ? 152 : 4;

    const PBELandmark left = point_or(face, li, {0.35f, 0.45f, 0});
    const PBELandmark right = point_or(face, ri, {0.65f, 0.45f, 0});
    const PBELandmark nose = point_or(face, ni, {0.50f, 0.52f, 0});
    const PBELandmark mouth = point_or(face, mi, {0.50f, 0.68f, 0});
    const PBELandmark chin = point_or(face, ci, {0.50f, 0.78f, 0});

    const float cx = (left.x + right.x) * 0.5f;
    const float cy = (left.y + right.y) * 0.5f;
    const float face_radius = std::clamp(
        std::hypot(right.x - left.x, right.y - left.y) * 1.48f,
        0.17f, 0.46f);

    const float slim = std::clamp(safe_float(p.face_slim), -0.45f, 0.45f);
    const float jaw = std::clamp(safe_float(p.jaw_width), -0.35f, 0.35f);
    const float eye = std::clamp(safe_float(p.eye_scale), -0.30f, 0.30f);
    const float eye_v = std::clamp(safe_float(p.eye_vertical), -0.15f, 0.15f);
    const float spacing = std::clamp(safe_float(p.eye_spacing), -0.25f, 0.25f);
    const float nose_scale = std::clamp(safe_float(p.nose_scale), -0.22f, 0.22f);
    const float nose_bridge = std::clamp(safe_float(p.nose_bridge), -0.15f, 0.15f);
    const float mouth_scale = std::clamp(safe_float(p.mouth_scale), -0.25f, 0.25f);
    const float mouth_v = std::clamp(safe_float(p.mouth_vertical), -0.15f, 0.15f);
    const float chin_len = std::clamp(safe_float(p.chin_length), -0.22f, 0.22f);

    if (std::fabs(slim)+std::fabs(jaw)+std::fabs(eye)+std::fabs(eye_v)+
        std::fabs(spacing)+std::fabs(nose_scale)+std::fabs(nose_bridge)+
        std::fabs(mouth_scale)+std::fabs(mouth_v)+std::fabs(chin_len) < 1e-4f) return;

    const ImageRGBA8 src = image;
    const int h = image.height;
    parallel_for_rows(h, threads, [&](int ys, int ye) {
        for (int y = ys; y < ye; ++y) {
            uint8_t* row = image.row(y);
            for (int x = 0; x < image.width; ++x) {
                const float u = static_cast<float>(x) / static_cast<float>(std::max(1, image.width - 1));
                const float v = static_cast<float>(y) / static_cast<float>(std::max(1, image.height - 1));
                const float nd = std::hypot((u - cx) * 0.92f, (v - cy));
                const float boundary = falloff(nd, face_radius * 1.16f);
                if (boundary <= 0.001f) continue;

                float sx = u;
                float sy = v;
                // Shrink the outer face while preserving the boundary ring.
                const float slim_amt = slim * 0.48f * boundary;
                sx += (u - cx) * slim_amt;
                sy += (v - cy) * slim_amt * 0.10f;

                // Jaw control is concentrated below the eyes.
                const float jaw_zone = falloff(std::hypot(u - cx, v - (cy + face_radius * 0.27f)), face_radius * 0.72f);
                sx += (u - cx) * jaw * 0.25f * jaw_zone;

                // Each eye is a separate local scale around the actual landmark.
                local_scale(u, v, left.x, left.y, 0.115f, -eye * 0.48f, sx, sy);
                local_scale(u, v, right.x, right.y, 0.115f, -eye * 0.48f, sx, sy);
                if (std::fabs(eye_v) > 1e-4f) {
                    const float wl = falloff(std::hypot(u-left.x, v-left.y), 0.11f);
                    const float wr = falloff(std::hypot(u-right.x, v-right.y), 0.11f);
                    sy -= eye_v * 0.08f * (wl + wr);
                }
                if (std::fabs(spacing) > 1e-4f) {
                    const float wl = falloff(std::hypot(u-left.x, v-left.y), 0.12f);
                    const float wr = falloff(std::hypot(u-right.x, v-right.y), 0.12f);
                    sx += (u < cx ? -1.0f : 1.0f) * spacing * 0.04f * std::max(wl, wr);
                }

                // Nose: local scale plus a constrained bridge lift.
                local_scale(u, v, nose.x, nose.y, 0.095f, -nose_scale * 0.35f, sx, sy);
                if (std::fabs(nose_bridge) > 1e-4f) {
                    const float w = falloff(std::hypot(u-nose.x, v-nose.y), 0.075f);
                    sy += nose_bridge * 0.018f * w;
                }

                // Mouth scale around mouth center, plus subtle vertical shift.
                local_scale(u, v, mouth.x, mouth.y, 0.105f, -mouth_scale * 0.40f, sx, sy);
                if (std::fabs(mouth_v) > 1e-4f) {
                    const float w = falloff(std::hypot(u-mouth.x, v-mouth.y), 0.11f);
                    sy -= mouth_v * 0.035f * w;
                }

                // Chin length: displacement attenuates into cheeks.
                if (std::fabs(chin_len) > 1e-4f) {
                    const float w = falloff(std::hypot(u-chin.x, v-chin.y), 0.14f);
                    sy -= chin_len * 0.065f * w;
                }

                // Quality affects blend strength, not geometry intent.
                const float q = quality == PBE_QUALITY_FAST ? 0.78f :
                                (quality == PBE_QUALITY_ULTRA ? 1.0f : 0.92f);
                const float blend = boundary * q;
                for (int c = 0; c < 3; ++c) {
                    const uint8_t warped = bilerp(src,
                        sx * static_cast<float>(src.width - 1),
                        sy * static_cast<float>(src.height - 1), c);
                    const float out = static_cast<float>(src.row(y)[x*4+c]) * (1.0f - blend) +
                                      static_cast<float>(warped) * blend;
                    row[x*4+c] = static_cast<uint8_t>(std::clamp(out, 0.0f, 255.0f));
                }
            }
        }
    });
}

void apply_body_geometry(ImageRGBA8& image, const PBEBodyData* body,
                         const PBEGeometryParams& p, PBERenderQuality quality, int threads) {
    if (!body || body->point_count < 8 || !body->points) return;

    const float slim = std::clamp(safe_float(p.body_slim), -0.28f, 0.28f);
    const float waist = std::clamp(safe_float(p.waist), -0.32f, 0.32f);
    const float shoulders = std::clamp(safe_float(p.shoulders), -0.22f, 0.22f);
    const float arms = std::clamp(safe_float(p.arms), -0.20f, 0.20f);
    const float legs = std::clamp(safe_float(p.legs), -0.22f, 0.22f);
    const float height = std::clamp(safe_float(p.height), -0.18f, 0.18f);
    if (std::fabs(slim)+std::fabs(waist)+std::fabs(shoulders)+std::fabs(arms)+std::fabs(legs)+std::fabs(height) < 1e-4f) return;

    // Expected generic pose order: nose, left/right shoulder, left/right elbow,
    // left/right wrist, left/right hip, left/right knee, left/right ankle.
    const PBEPosePoint ls = pose(body, 1), rs = pose(body, 2);
    const PBEPosePoint lh = pose(body, 7), rh = pose(body, 8);
    const PBEPosePoint lk = pose(body, 9), rk = pose(body, 10);

    const float shoulder_y = (ls.y + rs.y) * 0.5f;
    const float hip_y = (lh.y + rh.y) * 0.5f;
    const float body_h = std::max(0.20f, std::abs(pose(body, 11).y - pose(body, 0).y));

    const ImageRGBA8 src = image;
    parallel_for_rows(image.height, threads, [&](int ys, int ye) {
        for (int y = ys; y < ye; ++y) {
            uint8_t* row = image.row(y);
            for (int x = 0; x < image.width; ++x) {
                const float u = static_cast<float>(x) / static_cast<float>(std::max(1, image.width - 1));
                const float v = static_cast<float>(y) / static_cast<float>(std::max(1, image.height - 1));
                const float pm = person_weight(body, u, v);
                if (pm <= 0.02f) continue;

                const float torso_t = std::clamp((v - shoulder_y) / std::max(0.12f, hip_y - shoulder_y), 0.0f, 1.0f);
                const float torso_band = std::sin(torso_t * 3.14159265f);
                const float waist_band = std::exp(-std::pow((torso_t - 0.55f) / 0.30f, 2.0f));

                float sx = u;
                float sy = v;
                const float center = (1.0f - torso_t) * ((ls.x + rs.x) * 0.5f) +
                                     torso_t * ((lh.x + rh.x) * 0.5f);

                const float torso_width = std::max(0.10f, std::abs(rs.x - ls.x) * 1.55f +
                                                          std::abs(rh.x - lh.x) * 0.10f);
                const float side = u - center;
                const float width_w = std::clamp(1.0f - std::abs(side) / torso_width, 0.0f, 1.0f);

                sx += side * slim * 0.18f * torso_band * pm;
                sx += side * waist * 0.26f * waist_band * width_w * pm;
                sx += side * shoulders * 0.16f * (1.0f - torso_t) * pm;
                sx += side * arms * 0.10f * (0.35f + 0.65f * std::abs(0.5f - torso_t)) * pm;

                if (legs != 0.0f && v > hip_y) {
                    const float leg_zone = std::clamp((v - hip_y) / std::max(0.12f, 1.0f - hip_y), 0.0f, 1.0f);
                    const float width = std::max(0.05f, 0.5f * (std::abs(rk.x - rh.x) + std::abs(lk.x - lh.x)));
                    const float leg_fall = std::exp(-std::pow(side / (width + 0.05f), 2.0f));
                    sx += side * legs * 0.16f * leg_zone * leg_fall * pm;
                }

                if (height != 0.0f) {
                    const float top = shoulder_y - body_h * 0.32f;
                    const float t = std::clamp((v - top) / std::max(0.15f, 1.0f - top), 0.0f, 1.0f);
                    sy += height * 0.045f * (t - 0.5f) * pm;
                }

                const float q = quality == PBE_QUALITY_FAST ? 0.78f :
                                (quality == PBE_QUALITY_ULTRA ? 1.0f : 0.92f);
                const float blend = std::clamp(pm * q, 0.0f, 1.0f);
                for (int c = 0; c < 3; ++c) {
                    const uint8_t warped = bilerp(src,
                        sx * static_cast<float>(src.width - 1),
                        sy * static_cast<float>(src.height - 1), c);
                    row[x*4+c] = static_cast<uint8_t>(
                        std::clamp(static_cast<float>(src.row(y)[x*4+c])*(1.0f-blend) +
                                   static_cast<float>(warped)*blend, 0.0f, 255.0f));
                }
            }
        }
    });
}

} // namespace pbe::beauty
