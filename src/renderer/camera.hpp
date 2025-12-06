#pragma once

#include <algorithm>

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/ext/vector_float3.hpp"

namespace jengine::renderer {
class Camera {
  public:
    auto GetPosition() const { return position; }
    auto GetPitch() const { return pitch; }
    auto GetYaw() const { return yaw; }
    auto SetPosition(const glm::vec3& pos) { position = pos; }
    auto SetYaw(float y) { yaw = y; }
    auto SetPitch(float p) { pitch = p; }
    auto GetFOV() const { return fov; }
    auto SetFOV(float fov) { this->fov = fov; }
    auto GetAspectRatio() const { return aspect_ratio; }
    auto SetAspectRatio(float aspect_ratio) { this->aspect_ratio = aspect_ratio; }
    glm::mat4 ViewMatrix() const;
    glm::mat4 ProjectionMatrix() const;

    Camera(float fov,
           float aspect_ratio,
           const float pitch = 0,
           const float yaw = 0,
           const glm::vec3& position = {0, 0, 5})
        : fov(fov), aspect_ratio(aspect_ratio), pitch(pitch), yaw(yaw), position(position) {}

  private:
    float fov{};
    float aspect_ratio{};
    // vertical rotation
    float pitch{};
    // horizontal rotation
    float yaw{};
    glm::vec3 position{};

    void AddPitch(float rads) { pitch = std::clamp(pitch + rads, -glm::pi<float>() / 2, glm::pi<float>() / 2); }

    void AddYaw(float rads) { yaw += rads; }

    void AddPosition(const glm::vec3& pos) { position += pos; }
};
}  // namespace jengine::renderer
