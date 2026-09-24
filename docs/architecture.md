# Architecture v0.3

```text
Android App -> JNI -> libphoto_beauty.so -> C ABI -> C++ Core
                                             |
                       +---------------------+-------------------+
                       |                                         |
                  RenderGraph                                Backends
                       |                              Vulkan / AI Provider
             Color -> Beauty -> Local                 ncnn / OEM / custom
```

The public AI provider is model-agnostic; model weights remain external.

### v0.4 passes

```text
Tone -> Advanced Color -> Face Geometry -> Body Geometry -> Skin -> Local -> Heal -> Sharpen/Vignette/Grain
```
