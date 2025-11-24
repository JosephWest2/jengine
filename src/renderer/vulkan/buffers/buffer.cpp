#include "buffer.hpp"

#include "renderer/vulkan/memory_allocator.hpp"
#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan::buffers {

AllocatedBuffer::AllocatedBuffer(size_t size,
                                 vk::BufferUsageFlags usage_flags,
                                 VmaMemoryUsage memory_usage,
                                 const VmaAllocator allocator)
    : allocator(allocator) {
    vk::BufferCreateInfo buffer_create_info{
        .size = size,
        .usage = usage_flags,
        .sharingMode = vk::SharingMode::eExclusive,
    };

    VmaAllocationCreateInfo allocation_create_info{};
    allocation_create_info.usage = memory_usage;
    allocation_create_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VkBufferCreateInfo vk_buffer_create_info = buffer_create_info;
    VkBuffer buffer_result;
    vmaCreateBuffer(
        allocator, &vk_buffer_create_info, &allocation_create_info, &buffer_result, &allocation, &allocation_info);
};
AllocatedBuffer::~AllocatedBuffer() { vmaDestroyBuffer(allocator, buffer, allocation); }
}  // namespace jengine::renderer::vulkan::buffers
