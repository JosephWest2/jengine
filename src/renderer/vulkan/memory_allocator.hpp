#pragma once

#include <vulkan/vulkan_core.h>

#include "vk_mem_alloc.h"
#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan {
class MemoryAllocator {
  public:
    MemoryAllocator(const vk::Instance& instance,
                    const vk::Device& device,
                    const vk::PhysicalDevice& physical_device,
                    VmaAllocatorCreateFlags flags);
    ~MemoryAllocator();

    VmaAllocator& GetAllocator() { return allocator; }

  private:
    VmaAllocator allocator;
};
}  // namespace jengine::renderer::vulkan
