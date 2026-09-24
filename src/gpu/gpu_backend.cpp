#include "gpu/gpu_backend.h"

// The first CPU reference pipeline is deliberately independent of a graphics API.
// A Vulkan implementation plugs into this interface without changing the C ABI.
