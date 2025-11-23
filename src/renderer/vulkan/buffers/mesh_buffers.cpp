#include "mesh_buffers.hpp"

namespace jengine::renderer::vulkan::buffers {
MeshBuffers::MeshBuffers(const std::span<const uint32_t> indices,
                         const std::span<const Vertex> vertices,
                         const VmaAllocator allocator)
    : index_buffer(indices.size() * sizeof(uint32_t),
                   vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                   VMA_MEMORY_USAGE_GPU_ONLY,
                   allocator),
      vertex_buffer(vertices.size() * sizeof(Vertex),
                    vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                    VMA_MEMORY_USAGE_GPU_ONLY,
                    allocator) {}
}  // namespace jengine::renderer::vulkan::buffers
