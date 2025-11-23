#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "renderer/vulkan/physical_device.hpp"

namespace jengine::renderer::vulkan {

class Device {
  public:
    Device(vulkan::PhysicalDevice& physical_device);
    const vk::raii::Device& GetDevice() { return device; }
    const vk::Device& GetDeviceHandle() { return *device; }

    void WaitIdle() { device.waitIdle(); }
    void WaitForFences(const vk::ArrayProxy<const vk::Fence>& fences) {
        if (device.waitForFences(fences, true, TIMEOUT_ONE_SECOND) != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to wait for fence");
        }
    }
    void ResetFences(const vk::ArrayProxy<const vk::Fence>& fences) { device.resetFences(fences); }

  private:

    static constexpr uint64_t TIMEOUT_ONE_SECOND = 1000000000;

    vk::raii::Device device;

    static vk::raii::Device CreateDevice(vulkan::PhysicalDevice& physical_device);
};
}  // namespace jengine::renderer::vulkan
