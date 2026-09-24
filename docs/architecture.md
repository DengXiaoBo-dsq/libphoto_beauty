# Architecture

## Runtime boundary

`libphoto_beauty.so` exports only a stable C ABI. All implementation stays behind the ABI.

```text
Android/Kotlin
      |
     JNI
      |
libphoto_beauty.so
      |
      +-- Core / Image / Color
      +-- RenderGraph
      +-- Beauty / Mask
      +-- AI backend interface
      +-- GPU backend interface
```

## Data ownership

`PBEImageView` is caller-owned. The engine copies the source into its working buffer in v0.2.
Later versions may add explicit zero-copy handles for Android hardware buffers.

`PBEFaceData` is borrowed for the duration of a render call. The host must keep landmark/mask memory valid until rendering finishes.

## Quality tiers

- FAST: smallest neighborhood / lowest latency.
- HIGH: balanced local filtering.
- ULTRA: highest reference CPU path. Production GPU/AI passes will be substituted without changing the C ABI.

## Production direction

The next quality-critical layer is the learned vision stack:
face detector -> dense landmarks/mesh -> skin/person/hair parsing -> learned enhancement/restoration.
The model runtime is deliberately abstracted so the same engine can support ncnn, an OEM NPU runtime, or another backend.
