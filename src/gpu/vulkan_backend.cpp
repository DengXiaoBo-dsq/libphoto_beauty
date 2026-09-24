#ifdef PBE_ENABLE_VULKAN
#include "gpu/gpu_backend.h"
#include <vulkan/vulkan.h>

namespace pbe {

class VulkanBackend final : public GPUBackend {
public:
    bool available() const override { return true; }
    bool process(ImageRGBA8&) override {
        // Production implementation belongs here:
        // persistent VkInstance/VkDevice/VkQueue,
        // descriptor pools, compute pipelines, staging/imported images,
        // and pass scheduling from the render graph.
        return true;
    }
};

} // namespace pbe
#endif
