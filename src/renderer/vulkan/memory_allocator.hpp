#pragma once

#include <vulkan/vulkan_core.h>
#include "vk_mem_alloc.h"

namespace jengine::renderer::vulkan {
class MemoryAllocator {
    public:
        MemoryAllocator(VkInstance instance, VkDevice device, VkPhysicalDevice physical_device, VMA_);
};
}
