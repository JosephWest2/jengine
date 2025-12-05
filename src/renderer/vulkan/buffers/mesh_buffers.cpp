#include "mesh_buffers.hpp"

#include <cstring>

#include "renderer/vulkan/buffers/buffer.hpp"
#include "vk_mem_alloc.h"
#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan::buffers {
MeshBuffers::MeshBuffers(const std::span<const uint32_t> indices,
                         const std::span<const Vertex> vertices,
                         const VmaAllocator allocator,
                         const ImmediateSubmit& immediate_submit,
                         const vk::Device& device,
                         const vk::Queue& mesh_upload_queue)
    : index_buffer(indices.size() * sizeof(uint32_t),
                   vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                   VMA_MEMORY_USAGE_GPU_ONLY,
                   allocator),
      vertex_buffer(vertices.size() * sizeof(Vertex),
                    vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst |
                        vk::BufferUsageFlagBits::eShaderDeviceAddress,
                    VMA_MEMORY_USAGE_GPU_ONLY,
                    allocator) {
    UploadMeshData(vertices, indices, allocator, immediate_submit, device, mesh_upload_queue);
}
void MeshBuffers::UploadMeshData(const std::span<const Vertex> vertices,
                                 const std::span<const uint32_t> indices,
                                 const VmaAllocator allocator,
                                 const ImmediateSubmit& immediate_submit,
                                 const vk::Device& device,
                                 const vk::Queue& mesh_upload_queue) {
    AllocatedBuffer staging_buffer(index_buffer.GetSize() + vertex_buffer.GetSize(),
                                   vk::BufferUsageFlagBits::eTransferSrc,
                                   VMA_MEMORY_USAGE_CPU_ONLY,
                                   allocator);
    const size_t verticies_size = vertices.size() * sizeof(Vertex);
    const size_t indices_size = indices.size() * sizeof(uint32_t);

    if (verticies_size > vertex_buffer.GetSize()) {
        vertex_buffer = AllocatedBuffer(verticies_size,
                                        vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst |
                                            vk::BufferUsageFlagBits::eShaderDeviceAddress,
                                        VMA_MEMORY_USAGE_GPU_ONLY,
                                        allocator);
    }
    if (indices_size > index_buffer.GetSize()) {
        index_buffer = AllocatedBuffer(indices_size,
                                       vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                                       VMA_MEMORY_USAGE_GPU_ONLY,
                                       allocator);
    }
    vertex_buffer_address = device.getBufferAddress(vk::BufferDeviceAddressInfo{.buffer = vertex_buffer.GetBuffer()});

    void* data = staging_buffer.GetMappedData();
    memcpy(data, vertices.data(), vertex_buffer.GetSize());
    memcpy((char*)data + vertex_buffer.GetSize(), indices.data(), index_buffer.GetSize());

    immediate_submit.Submit(device, mesh_upload_queue, [&](const vk::CommandBuffer& command_buffer) {
        vk::BufferCopy vertex_buffer_copy{
            .srcOffset = 0,
            .dstOffset = 0,
            .size = vertex_buffer.GetSize(),
        };
        command_buffer.copyBuffer(staging_buffer.GetBuffer(), vertex_buffer.GetBuffer(), vertex_buffer_copy);

        vk::BufferCopy index_buffer_copy{
            .srcOffset = vertex_buffer.GetSize(),
            .dstOffset = 0,
            .size = index_buffer.GetSize(),
        };
        command_buffer.copyBuffer(staging_buffer.GetBuffer(), index_buffer.GetBuffer(), index_buffer_copy);
    });
}
MeshBuffers::MeshBuffers(MeshBuffers&& other)
    : index_buffer(std::move(other.index_buffer)),
      vertex_buffer(std::move(other.vertex_buffer)),
      vertex_buffer_address(other.vertex_buffer_address) {
    other.vertex_buffer_address = 0;
};
MeshBuffers& MeshBuffers::operator=(MeshBuffers&& other) {
    index_buffer = std::move(other.index_buffer);
    vertex_buffer = std::move(other.vertex_buffer);
    other.vertex_buffer_address = 0;
    return *this;
};
}  // namespace jengine::renderer::vulkan::buffers
