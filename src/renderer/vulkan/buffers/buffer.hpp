#pragma once

#include "renderer/vulkan/memory_allocator.hpp"
#include "vulkan/vulkan.hpp"
namespace jengine::renderer::vulkan::buffers {
class Buffer {
  public:
    Buffer(size_t size, vk::BufferUsageFlags usage_flags, VmaMemoryUsage memory_usage, const VmaAllocator allocator);
    ~Buffer();

    const vk::Buffer& GetBuffer() const { return buffer; }

  private:
    vk::Buffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo allocation_info;

    // held for destruction
    const VmaAllocator allocator;
};
}  // namespace jengine::renderer::vulkan::buffers
