#pragma once

#include <vulkan/vulkan_core.h>

#include "vk_mem_alloc.h"

namespace jengine::renderer::vulkan {
class MemoryAllocator {
  public:
    MemoryAllocator(VkInstance instance,
                    VkDevice& device,
                    VkPhysicalDevice physical_device,
                    VmaAllocatorCreateFlags flags);
    ~MemoryAllocator();

    VmaAllocator& GetAllocator() { return allocator; }

  private:
    VmaAllocator allocator;
};
}  // namespace jengine::renderer::vulkan
