#include "memory_allocator.hpp"

#include "vk_mem_alloc.h"

namespace jengine::renderer::vulkan {

MemoryAllocator::MemoryAllocator(const vk::Instance& instance,
                                 const vk::Device& device,
                                 const vk::PhysicalDevice& physical_device,
                                 VmaAllocatorCreateFlags flags) {
    VmaAllocatorCreateInfo create_info = {};
    create_info.instance = instance;
    create_info.physicalDevice = physical_device;
    create_info.device = device;
    create_info.flags = flags;

    vmaCreateAllocator(&create_info, &allocator);
}

MemoryAllocator::~MemoryAllocator() { vmaDestroyAllocator(allocator); }
}  // namespace jengine::renderer::vulkan
