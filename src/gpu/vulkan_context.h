#pragma once
namespace pbe { class VulkanContext { public: bool initialize(); void shutdown(); bool available() const { return initialized_; } private: bool initialized_=false; void* instance_=nullptr; }; }
