#pragma once

#include "VkBootstrap.h"
#include "renderer/vulkan/deletion_stack.hpp"
namespace jengine::renderer::vulkan {
class Device {
  public:
    Device(vkb::PhysicalDevice& physical_device, DeletionStack& deletion_stack);

    VkDevice GetDevice() const { return vkb_device.device; }
    vkb::Device& GetVkbDevice() { return vkb_device; }

    void Destroy();

  private:
    vkb::Device vkb_device;
};
}  // namespace jengine::renderer::vulkan
