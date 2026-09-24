#include "gpu/vulkan_context.h"
#ifdef PBE_ENABLE_VULKAN
#include <vulkan/vulkan.h>
#endif
namespace pbe {
bool VulkanContext::initialize(){
#ifdef PBE_ENABLE_VULKAN
 if(initialized_)return true; VkApplicationInfo app{VK_STRUCTURE_TYPE_APPLICATION_INFO,nullptr,"PhotoBeautyEngine",VK_MAKE_VERSION(0,3,0),"PhotoBeautyEngine",VK_MAKE_VERSION(0,3,0),VK_API_VERSION_1_1}; VkInstanceCreateInfo ci{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,nullptr,0,&app,0,nullptr,0,nullptr}; VkInstance inst=VK_NULL_HANDLE; if(vkCreateInstance(&ci,nullptr,&inst)!=VK_SUCCESS)return false; uint32_t n=0; vkEnumeratePhysicalDevices(inst,&n,nullptr); if(n==0){vkDestroyInstance(inst,nullptr);return false;} instance_=reinterpret_cast<void*>(inst); initialized_=true; return true;
#else
 return false;
#endif
}
void VulkanContext::shutdown(){
#ifdef PBE_ENABLE_VULKAN
 if(instance_)vkDestroyInstance(reinterpret_cast<VkInstance>(instance_),nullptr);
#endif
 instance_=nullptr;initialized_=false;
}
}
