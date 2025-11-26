#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "glm/fwd.hpp"
#include "renderer/vulkan/buffers/buffer.hpp"
#include "renderer/vulkan/immediate_submit.hpp"
#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan::buffers {
struct Vertex {
    glm::vec3 position{};
    float uv_x{};
    glm::vec3 normal{};
    float uv_y{};
    glm::vec4 color{};
};

constexpr std::array<Vertex, 4> test_vertices = {Vertex{
                                                     .position = glm::vec3(0.5f, -0.5f, 0.0f),
                                                     .color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
                                                 },
                                                 Vertex{
                                                     .position = glm::vec3(0.5f, 0.5f, 0.0f),
                                                     .color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
                                                 },
                                                 Vertex{
                                                     .position = glm::vec3(-0.5f, -0.5f, 0.0f),
                                                     .color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
                                                 },
                                                 Vertex{
                                                     .position = glm::vec3(-0.5f, 0.5f, 0.0f),
                                                     .color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                                                 }};

constexpr std::array<uint32_t, 6> test_indices = {0, 1, 2, 2, 1, 3};

struct MeshDrawPushConstants {
    glm::mat4 world_matrix;
    vk::DeviceAddress vertex_buffer_address;
};

class MeshBuffers {
  public:
    MeshBuffers(const std::span<const uint32_t> indices,
                const std::span<const Vertex> vertices,
                const VmaAllocator allocator,
                const ImmediateSubmit& immediate_submit,
                const vk::Device& device,
                const vk::Queue& mesh_upload_queue);

    // takes an allocator to allocate a staging buffer
    void UploadMeshData(const std::span<const Vertex> vertices,
                        const std::span<const uint32_t> indices,
                        const VmaAllocator,
                        const ImmediateSubmit& immediate_submit,
                        const vk::Device& device,
                        const vk::Queue& mesh_upload_queue);

    const vk::DeviceAddress& GetVertexBufferAddress() const { return vertex_buffer_address; }

    const AllocatedBuffer& GetIndexBuffer() const { return index_buffer; }
    const AllocatedBuffer& GetVertexBuffer() const { return vertex_buffer; }

  private:
    static constexpr size_t MIN_PUSH_CONSTANTS_SIZE = 128;
    static_assert(sizeof(MeshDrawPushConstants) <= MIN_PUSH_CONSTANTS_SIZE);

    AllocatedBuffer index_buffer;
    AllocatedBuffer vertex_buffer;
    vk::DeviceAddress vertex_buffer_address;
};
}  // namespace jengine::renderer::vulkan::buffers
