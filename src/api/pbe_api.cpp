#include "pbe/pbe.h"
#include "core/engine.h"

#include <new>
#include <cstring>

extern "C" {

PBEStatus pbe_create(const PBEConfig* config, PBEHandle* out_handle) {
    if (!out_handle) return PBE_INVALID_ARGUMENT;

    PBEConfig cfg{};
    cfg.quality = PBE_QUALITY_HIGH;
    cfg.prefer_gpu = 1;
    cfg.thread_count = 0;
    cfg.max_working_memory = 0;
    if (config) cfg = *config;

    try {
        auto* engine = new pbe::Engine(cfg);
        *out_handle = reinterpret_cast<PBEHandle>(engine);
        return PBE_OK;
    } catch (const std::bad_alloc&) {
        return PBE_OUT_OF_MEMORY;
    } catch (...) {
        return PBE_INTERNAL_ERROR;
    }
}

void pbe_destroy(PBEHandle handle) {
    delete reinterpret_cast<pbe::Engine*>(handle);
}

PBEStatus pbe_reset(PBEHandle handle) {
    if (!handle) return PBE_INVALID_ARGUMENT;
    return reinterpret_cast<pbe::Engine*>(handle)->reset();
}

PBEStatus pbe_set_beauty_params(PBEHandle handle, const PBEBeautyParams* params) {
    if (!handle || !params) return PBE_INVALID_ARGUMENT;
    return reinterpret_cast<pbe::Engine*>(handle)->set_beauty(*params);
}

PBEStatus pbe_get_beauty_params(PBEHandle handle, PBEBeautyParams* out_params) {
    if (!handle || !out_params) return PBE_INVALID_ARGUMENT;
    *out_params = reinterpret_cast<pbe::Engine*>(handle)->beauty();
    return PBE_OK;
}

PBEStatus pbe_set_auto_settings(PBEHandle handle, const PBEAutoSettings* settings) {
    if (!handle || !settings) return PBE_INVALID_ARGUMENT;
    return reinterpret_cast<pbe::Engine*>(handle)->set_auto(*settings);
}

PBEStatus pbe_set_face_data(PBEHandle handle, const PBEFaceData* face_data) {
    if (!handle) return PBE_INVALID_ARGUMENT;
    return reinterpret_cast<pbe::Engine*>(handle)->set_face_data(face_data);
}

PBEStatus pbe_render_rgba8(
    PBEHandle handle,
    const PBEImageView* input,
    PBEImageView* output
) {
    if (!handle || !input || !output) return PBE_INVALID_ARGUMENT;
    return reinterpret_cast<pbe::Engine*>(handle)->render(*input, *output);
}

PBEStatus pbe_auto_enhance(PBEHandle handle) {
    if (!handle) return PBE_INVALID_ARGUMENT;
    return reinterpret_cast<pbe::Engine*>(handle)->auto_enhance();
}

const char* pbe_get_last_error(PBEHandle handle) {
    if (!handle) return "Invalid handle.";
    return reinterpret_cast<pbe::Engine*>(handle)->error().c_str();
}

} // extern "C"
