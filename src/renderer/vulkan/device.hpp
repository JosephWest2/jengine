#pragma once

#include "VkBootstrap.h"

namespace jengine::renderer::vulkan {
class Device {
  public:
    Device(vkb::PhysicalDevice& physical_device);
    ~Device();

    VkDevice& GetDevice() { return vkb_device.device; }
    vkb::Device& GetVkbDevice() { return vkb_device; }

  private:
    vkb::Device vkb_device;
};
}  // namespace jengine::renderer::vulkan
