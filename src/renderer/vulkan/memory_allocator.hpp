#pragma once

#include <vulkan/vulkan_core.h>

#include "renderer/vulkan/deletion_stack.hpp"
#include "vk_mem_alloc.h"

namespace jengine::renderer::vulkan {
class MemoryAllocator {
  public:
    MemoryAllocator(VkInstance instance,
                    VkDevice device,
                    VkPhysicalDevice physical_device,
                    VmaAllocatorCreateFlags flags,
                    DeletionStack& deletion_stack);

    VmaAllocator GetAllocator() const { return allocator; }

    void Destroy();

  private:
    VmaAllocator allocator;
};
}  // namespace jengine::renderer::vulkan
