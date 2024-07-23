#include "camera.hpp"

#include "log.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

Camera::Camera() : origin(0.0, 0.0, 0.0), direction(0.0, 0.0, 1.0),
  up(0.0, 1.0, 0.0), fovY(glm::half_pi<float>()),
  nearPlane(0.1f), farPlane(5000.0f) {
    LOG_DEBUG("Created camera");
}

Camera::~Camera() {
    LOG_DEBUG("Destroyed camera");
}

glm::mat4 Camera::viewMatrix() {
    return glm::lookAt(origin, origin + direction, up);
}

glm::mat4 Camera::projectionMatrix(float aspect) {
    return glm::perspective(fovY, aspect, nearPlane, farPlane);
}
