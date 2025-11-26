#include "buffer.hpp"

#include <vulkan/vulkan_core.h>

#include "renderer/vulkan/memory_allocator.hpp"
#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan::buffers {

AllocatedBuffer::AllocatedBuffer(size_t size,
                                 vk::BufferUsageFlags usage_flags,
                                 VmaMemoryUsage memory_usage,
                                 const VmaAllocator allocator)
    : size(size), allocator(allocator) {
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
    buffer = buffer_result;
};
AllocatedBuffer::~AllocatedBuffer() { Destroy(); }
AllocatedBuffer::AllocatedBuffer(AllocatedBuffer&& other) noexcept
    : size(other.size),
      buffer(other.buffer),
      allocation(other.allocation),
      allocation_info(other.allocation_info),
      allocator(other.allocator) {
    other.size = 0;
    other.buffer = nullptr;
    other.allocation = nullptr;
    other.allocator = nullptr;
    other.allocation_info = {};
}
AllocatedBuffer& AllocatedBuffer::operator=(AllocatedBuffer&& other) noexcept {
    if (this != &other) {
        Destroy();
        size = other.size;
        buffer = other.buffer;
        allocation = other.allocation;
        allocation_info = other.allocation_info;
        allocator = other.allocator;
        other.size = 0;
        other.buffer = nullptr;
        other.allocation = nullptr;
        other.allocation_info = {};
        other.allocator = nullptr;
    }
    return *this;
}
void AllocatedBuffer::Destroy() {
    if (buffer) {
        vmaDestroyBuffer(allocator, buffer, allocation);
    }
}
}  // namespace jengine::renderer::vulkan::buffers
