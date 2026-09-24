#!/usr/bin/env bash
set -euo pipefail

# Pinning by source tag avoids coupling the engine to a prebuilt NDK version.
# Override NCNN_TAG to update after validating with the project's NDK.
NCNN_TAG="${NCNN_TAG:-20260526}"
DEST="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/ncnn"

if [[ -d "${DEST}/.git" ]]; then
  git -C "${DEST}" fetch --tags
  git -C "${DEST}" checkout "${NCNN_TAG}"
else
  git clone --depth 1 --branch "${NCNN_TAG}" https://github.com/Tencent/ncnn.git "${DEST}"
fi

echo "ncnn source prepared at ${DEST} (${NCNN_TAG})"
