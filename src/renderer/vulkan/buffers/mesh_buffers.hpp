#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "glm/fwd.hpp"
#include "renderer/vulkan/buffers/buffer.hpp"
#include "vulkan/vulkan.hpp"

namespace jengine::renderer::vulkan::buffers {
class MeshBuffers {
  public:
    struct Vertex {
        glm::vec3 position;
        float uv_x;
        glm::vec3 normal;
        float uv_y;
        glm::vec4 color;
    };

    struct DrawPushConstants {
        glm::mat4 world_matrix;
        vk::DeviceAddress vertex_buffer_address;
    };

    MeshBuffers(const std::span<const uint32_t> indices,
                const std::span<const Vertex> vertices,
                const VmaAllocator allocator);

  private:
    static constexpr size_t MIN_PUSH_CONSTANTS_SIZE = 128;
    static_assert(sizeof(DrawPushConstants) <= MIN_PUSH_CONSTANTS_SIZE);

    Buffer index_buffer;
    Buffer vertex_buffer;
    vk::DeviceAddress vertex_buffer_address;
};
}  // namespace jengine::renderer::vulkan::buffers
