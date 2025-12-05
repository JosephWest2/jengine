#pragma once

#include <algorithm>

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/ext/vector_float3.hpp"

namespace jengine::renderer {
struct Camera {
    float fov{75};
    float aspect_ratio{16.0f / 9.0f};
    float near{0};
    float far{1000};
    // vertical rotation
    float pitch{};
    // horizontal rotation
    float yaw{};
    glm::vec3 position{0, 0, -5};

    void AddPitch(float rads) { pitch = std::clamp(pitch + rads, -glm::pi<float>() / 2, glm::pi<float>() / 2); }

    void AddYaw(float rads) { yaw += rads; }

    void AddPosition(const glm::vec3& pos) { position += pos; }

    glm::mat4 ViewMatrix() const;

    glm::mat4 ProjectionMatrix() const;
};
}  // namespace jengine::renderer
