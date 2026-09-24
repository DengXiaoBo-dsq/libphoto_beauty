# C API

The ABI is intentionally C-compatible so Kotlin/Java only needs a thin JNI layer.

```c
PBEHandle engine;
PBEConfig cfg = { PBE_QUALITY_HIGH, 1, 0, 0 };
pbe_create(&cfg, &engine);

PBEBeautyParams p;
pbe_get_beauty_params(engine, &p);
p.skin_smooth = 0.20f;
p.face_slim = 0.08f;
pbe_set_beauty_params(engine, &p);

pbe_render_rgba8(engine, &input, &output);
pbe_destroy(engine);
```

`PBEFaceData` can supply a dense face landmark array plus learned masks. The engine does not own those buffers.
