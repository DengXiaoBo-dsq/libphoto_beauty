# Photo Beauty Engine

A C/C++ photo beauty core designed to be wrapped by a thin Android JNI/AAR layer.

## Current scope

This repository is the production-oriented foundation:

- Stable C ABI
- C++17 core
- Non-UI library
- RGBA8/BGRA8/RGB8 ingestion
- Auto exposure / tone baseline
- Professional-ish tone controls
- Skin smoothing with optional learned mask input
- Conservative heuristic skin fallback
- Sharpen / vignette / grain
- AI backend abstraction
- GPU backend abstraction
- Android NDK / CMake build
- Native unit test

## Important quality note

This is a real, compilable foundation. It is NOT the claim that a few thousand lines of CPU filters
already equal a commercial AI beauty engine.

Top-tier visual quality requires model assets and additional production modules, especially:

1. Face detector / dense landmarks
2. Skin / hair / person parsing
3. Mesh-based face and body warping
4. AI face restoration and denoise
5. GPU render graph
6. Device-specific NPU/CPU/GPU execution
7. HDR / wide-gamut / 10-bit pipeline
8. Extensive visual regression tests across device classes

Those modules are deliberately isolated so the ABI and engine architecture do not need to be rewritten.

## Build on desktop

Requirements:
- CMake 3.22+
- Ninja
- C++17 compiler

```bash
./scripts/build_desktop.sh
```

## Build for Android

The pinned Android NDK target for this project is r27c / 27.2.12479018, with arm64-v8a as the primary ABI.

```bash
export ANDROID_SDK_ROOT=$HOME/Android/Sdk
export ANDROID_NDK_HOME=$ANDROID_SDK_ROOT/ndk/27.2.12479018

./scripts/build_android.sh
```

For a device-oriented Vulkan build:

```bash
PBE_ENABLE_VULKAN=ON ./scripts/build_android.sh
```

## ABI

The exported ABI is C, so the Android layer can stay tiny:

```text
Kotlin/Java
   -> JNI
      -> libphoto_beauty.so
         -> C ABI
            -> C++ Engine
```

## Example C API usage

```cpp
PBEConfig cfg{};
cfg.quality = PBE_QUALITY_HIGH;
cfg.prefer_gpu = 1;

PBEHandle engine = nullptr;
pbe_create(&cfg, &engine);

PBEBeautyParams p{};
pbe_get_beauty_params(engine, &p);
p.strength = 1.0f;
p.skin_smooth = 0.20f;
p.skin_bright = 0.05f;
p.exposure = 0.10f;
p.vibrance = 0.05f;
p.sharpen = 0.10f;
pbe_set_beauty_params(engine, &p);

pbe_render_rgba8(engine, &input, &output);
pbe_destroy(engine);
```

## Architecture

```text
                    libphoto_beauty.so
                           |
                  +--------+--------+
                  |                 |
               C ABI             C++ Core
                                    |
                           +--------+--------+
                           |                 |
                       CPU Pipeline       AI/GPU
                           |                 |
                    +------+-----+      +----+----+
                    |            |      |         |
                 Color        Beauty   ncnn     Vulkan
                    |            |
                 Tone/HSL     Skin/Mask
```
