# Third-party policy

The core library intentionally starts with no mandatory third-party runtime.

Recommended production integrations:

- ncnn: mobile AI inference backend.
- Vulkan: GPU compute/render backend.
- Optional image codecs can be supplied by the host app.

For AI models, keep model files outside the core source tree and load them at runtime.
Do not assume a model's repository license is the same as the model-weight license.
