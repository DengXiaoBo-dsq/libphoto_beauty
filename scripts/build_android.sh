#!/usr/bin/env bash
set -euo pipefail

: "${ANDROID_NDK_HOME:?Set ANDROID_NDK_HOME to your NDK r27c directory (27.2.12479018)}"
: "${ANDROID_SDK_ROOT:?Set ANDROID_SDK_ROOT to your Android SDK directory}"

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD="${ROOT}/build/android"

ABI="${ABI:-arm64-v8a}"
API="${API:-24}"

cmake -S "${ROOT}" -B "${BUILD}/${ABI}" \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE="${ANDROID_NDK_HOME}/build/cmake/android.toolchain.cmake" \
  -DANDROID_ABI="${ABI}" \
  -DANDROID_PLATFORM="android-${API}" \
  -DPBE_BUILD_TESTS=OFF \
  -DPBE_BUILD_SHARED=ON \
  -DPBE_ENABLE_VULKAN="${PBE_ENABLE_VULKAN:-OFF}"

cmake --build "${BUILD}/${ABI}" --target PhotoBeautyEngine -j
echo "Built: ${BUILD}/${ABI}/libphoto_beauty.so"

# Expected NDK revision for this project: r27c / 27.2.12479018
