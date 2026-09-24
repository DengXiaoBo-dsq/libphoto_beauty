#!/usr/bin/env bash
set -euo pipefail

# ncnn 20241226 is the maintained Android release in this project line that
# explicitly used android-ndk-r27c for its prebuilt Android packages.
NCNN_TAG="${NCNN_TAG:-20241226}"
DEST="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/ncnn"

if [[ -d "${DEST}/.git" ]]; then
  git -C "${DEST}" fetch --tags
  git -C "${DEST}" checkout "${NCNN_TAG}"
else
  git clone --depth 1 --branch "${NCNN_TAG}" https://github.com/Tencent/ncnn.git "${DEST}"
fi

echo "ncnn source prepared at ${DEST} (${NCNN_TAG})"
