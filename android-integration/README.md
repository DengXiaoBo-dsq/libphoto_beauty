# Android integration contract

The native library exposes a C ABI and can be packaged into an AAR without putting any rendering logic in Kotlin.

Recommended layout in the host app:

```text
app/src/main/cpp/include/pbe/      <- public headers
app/src/main/jniLibs/arm64-v8a/libphoto_beauty.so
```

A thin JNI layer should:
1. obtain RGBA pixels from the host image pipeline,
2. pass direct buffers/pointers into `pbe_render_rgba8`,
3. keep the PBEHandle alive for the editing session,
4. destroy it when the editor closes.

For camera/HW-buffer zero-copy and production Vulkan compute, build the AI/GPU backend against Android API 26+ and keep ownership/lifetime at the native layer.
