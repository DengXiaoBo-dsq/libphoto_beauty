# Third-party components

The core v0.2 source has no mandatory third-party runtime.

Recommended optional production components:

- ncnn: mobile inference runtime; the official project provides Android CPU and Vulkan packages. See its current GitHub releases and license before redistributing. Do not copy model weights without checking their individual terms.
- MediaPipe: useful during research/validation for face/vision tasks; keep it behind an internal adapter if used.

Model repositories and model weights are separate licensing questions. A permissive code license does not automatically grant redistribution rights to pretrained weights.
