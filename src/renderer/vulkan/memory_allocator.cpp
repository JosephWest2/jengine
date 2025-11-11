#include "memory_allocator.hpp"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

namespace jengine::renderer::vulkan {

MemoryAllocator::MemoryAllocator(VkInstance instance, VkDevice device, VkPhysicalDevice physical_device, VmaAllocatorCreateFlags flags) {
    VmaAllocatorCreateInfo create_info = {};
    create_info.instance = instance;
    create_info.physicalDevice = physical_device;
    create_info.device = device;
    create_info.flags = flags;

    vmaCreateAllocator(&create_info, &allocator);
}

void MemoryAllocator::Destroy() {
    vmaDestroyAllocator(allocator);
}
}  // namespace jengine::renderer::vulkan
