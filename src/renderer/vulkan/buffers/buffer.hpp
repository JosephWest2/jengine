#pragma once

#include "renderer/vulkan/memory_allocator.hpp"
#include "vulkan/vulkan.hpp"
namespace jengine::renderer::vulkan::buffers {
class AllocatedBuffer {
  public:
    AllocatedBuffer(size_t size,
                    vk::BufferUsageFlags usage_flags,
                    VmaMemoryUsage memory_usage,
                    const VmaAllocator allocator);
    ~AllocatedBuffer();

    AllocatedBuffer(const AllocatedBuffer&) = delete;
    AllocatedBuffer& operator=(const AllocatedBuffer&) = delete;

    AllocatedBuffer(AllocatedBuffer&& other) noexcept;
    AllocatedBuffer& operator=(AllocatedBuffer&& other) noexcept;

    const vk::Buffer& GetBuffer() const { return buffer; }
    void* GetMappedData() const { return allocation_info.pMappedData; }
    size_t GetSize() const { return size; }

  private:
    size_t size;
    vk::Buffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo allocation_info;

    // held for destruction
    VmaAllocator allocator;

    void Destroy();
};
}  // namespace jengine::renderer::vulkan::buffers
