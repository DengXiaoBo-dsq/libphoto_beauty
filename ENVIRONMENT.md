# Build environment

## Android
- Android Studio current stable
- Android SDK platform 35 or newer
- CMake 3.22+
- Ninja
- Android NDK r27c: 27.2.12479018
- Primary ABI: arm64-v8a
- Native API floor used by the Vulkan/HW-buffer path: API 26+

## Desktop reference build
- CMake 3.22+
- Ninja
- C++17 compiler

## Commands

```bash
./scripts/build_desktop.sh

export ANDROID_SDK_ROOT=$HOME/Android/Sdk
export ANDROID_NDK_HOME=$ANDROID_SDK_ROOT/ndk/27.2.12479018
./scripts/build_android.sh
```
