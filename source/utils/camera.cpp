#include "camera.h"

Camera::Camera()
    : position(0.0f, 0.0f, 3.0f), // Default: 3 units back
      rotation(0.0f, 0.0f, 0.0f), // No rotation
      fov(45.0f),                 // Default FOV
      aspect(1.0f),               // Default aspect ratio
      nearClip(0.1f),             // Near plane
      farClip(100.0f)             // Far plane
{
}

glm::mat4 Camera::getViewMatrix() const {
    glm::mat4 view = glm::mat4(1.0f);
    // Apply rotations (pitch, yaw, roll)
    view = glm::rotate(view, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::rotate(view, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    // Translate to position
    view = glm::translate(view, -position);
    return view;
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), aspect, nearClip, farClip);
}

void Camera::setAspect(float asp) {
    aspect = asp;
}
