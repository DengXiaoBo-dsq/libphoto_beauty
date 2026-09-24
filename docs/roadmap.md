# Roadmap to commercial-grade quality

## v0.2 (this package)
- C ABI and C++17 core
- RenderGraph
- scene analysis
- presets
- mask operators
- face landmark radial warp
- edge-aware skin processing
- optional ncnn seam

## v0.3
- production Vulkan render graph and persistent GPU resources
- model registry and ncnn execution backend
- dense face mesh adapter
- learned skin/person/hair masks

## v0.4
- mesh-based face deformation with boundary constraints
- eye/nose/mouth region warps
- body mesh and pose-aware deformation
- blemish/heal pipeline

## v0.5
- face restoration + denoise + super resolution
- HDR / wide-gamut / 10-bit path
- camera zero-copy path where supported

## v1.0
- visual regression suite across devices
- memory/latency budgets
- ABI stability policy
- release packaging and Android AAR helper

The version boundary is based on validated image quality and performance, not line count.
