#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD="${ROOT}/build/desktop"

cmake -S "${ROOT}" -B "${BUILD}" \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DPBE_BUILD_TESTS=ON \
  -DPBE_BUILD_SHARED=ON

cmake --build "${BUILD}" -j
ctest --test-dir "${BUILD}" --output-on-failure
