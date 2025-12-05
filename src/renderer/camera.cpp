#include "camera.hpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace jengine::renderer {

glm::mat4 Camera::ViewMatrix() const {
    glm::mat4 view_matrix = glm::identity<glm::mat4>();
    view_matrix *= glm::rotate(view_matrix, static_cast<float>(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    view_matrix *= glm::rotate(view_matrix, static_cast<float>(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    view_matrix *= glm::translate(view_matrix, -position);
    return view_matrix;
}
glm::mat4 Camera::ProjectionMatrix() const { return glm::perspective(glm::radians(fov), aspect_ratio, near, far); }
}  // namespace jengine::renderer
