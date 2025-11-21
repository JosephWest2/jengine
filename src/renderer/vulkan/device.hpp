#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "renderer/vulkan/physical_device.hpp"

namespace jengine::renderer::vulkan {

class Device {
  public:
    Device(vulkan::PhysicalDevice& physical_device);
    vk::raii::Device& GetDevice() { return device; }
    const vk::Device& GetDeviceHandle() { return *device; }

  private:
    vk::raii::Device device;

    static vk::raii::Device CreateDevice(vulkan::PhysicalDevice& physical_device);
};
}  // namespace jengine::renderer::vulkan
