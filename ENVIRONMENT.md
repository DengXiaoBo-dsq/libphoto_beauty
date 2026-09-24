# Build environment

## Android
- Android Studio current stable
- Android SDK platform 35+
- CMake 3.22+
- Ninja
- Android NDK r27c: 27.2.12479018
- Primary ABI: arm64-v8a
- API floor: 26
- C++: C++17

## AI/GPU
- Vulkan: optional production backend
- ncnn: optional production AI runtime; default project tag 20241226 for r27c compatibility

## Desktop reference
- CMake 3.22+
- Ninja
- C++17 compiler

## Commands

```bash
./scripts/build_desktop.sh

export ANDROID_SDK_ROOT=$HOME/Android/Sdk
export ANDROID_NDK_HOME=$ANDROID_SDK_ROOT/ndk/27.2.12479018
./scripts/build_android.sh

# Optional ncnn source
./third_party/fetch_ncnn.sh
cmake -S . -B build/android -DPBE_ENABLE_NCNN=ON
```
