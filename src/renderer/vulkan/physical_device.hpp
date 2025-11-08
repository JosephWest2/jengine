#pragma once

#include <vulkan/vulkan_core.h>
#include "VkBootstrap.h"
namespace jengine::renderer::vulkan {

class PhysicalDevice {
  public:
    PhysicalDevice(VkSurfaceKHR surface, vkb::Instance& instance);

    VkPhysicalDevice GetPhysicalDevice() const { return vkb_physical_device.physical_device; }
    vkb::PhysicalDevice& GetVkbPhysicalDevice() { return vkb_physical_device; }
  private:
    vkb::PhysicalDevice vkb_physical_device{};
};
}  // namespace jengine::renderer::vulkan
