# API v0.3

Public header: `include/pbe/pbe.h`.

Added `pbe_set_advanced_color`, `pbe_get_advanced_color`, and `pbe_set_ai_provider`.
The provider's returned mask/landmark buffers must remain valid until the enclosing call returns.
