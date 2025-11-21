#pragma once

#include <vulkan/vulkan_core.h>

#include <vulkan/vulkan_raii.hpp>

#include "absl/container/flat_hash_map.h"
#include "vulkan/vulkan.hpp"
#include <vulkan/vulkan_hash.hpp>

namespace jengine::renderer::vulkan {

class PhysicalDevice {
  public:
    PhysicalDevice(vk::raii::Instance& instance);

    vk::raii::PhysicalDevice& GetPhysicalDevice() { return physical_device; }
    uint32_t GetQueueFamilyIndex(vk::QueueFlagBits queue_flags);
    uint32_t GetPresentQueueIndex(const vk::SurfaceKHR& surface);

  private:
    vk::raii::PhysicalDevice physical_device;
    absl::flat_hash_map<vk::QueueFlagBits, uint32_t> index_cache{};
    absl::flat_hash_map<vk::SurfaceKHR, uint32_t> present_index_cache{};

    static vk::raii::PhysicalDevice CreatePhysicalDevice(vk::raii::Instance& instance);
    static std::optional<uint32_t> GetPhysicalDeviceQueueFamilyIndex(const vk::raii::PhysicalDevice& physical_device,
                                                                     vk::QueueFlagBits queue_flags);
};
}  // namespace jengine::renderer::vulkan
