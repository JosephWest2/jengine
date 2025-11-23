#pragma once

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_raii.hpp>

#include "vk_mem_alloc.h"

namespace jengine::renderer::vulkan {
class MemoryAllocator {
  public:
    MemoryAllocator(const vk::raii::Instance& instance,
                    const vk::raii::Device& device,
                    const vk::raii::PhysicalDevice& physical_device,
                    VmaAllocatorCreateFlags flags);
    ~MemoryAllocator();

    VmaAllocator& GetAllocator() { return allocator; }

  private:
    VmaAllocator allocator;
};
}  // namespace jengine::renderer::vulkan
