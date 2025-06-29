
#include "gamecamera.h"
#include <glm/gtc/matrix_transform.hpp>

GameCamera::GameCamera(const std::string& name) : name(name), position(0.0f, 0.0f, 3.0f), rotation(0.0f), fov(45.0f) {}

glm::vec3 GameCamera::getForward() const {
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::mat4 rot = glm::mat4(1.0f);
    rot = glm::rotate(rot, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw
    rot = glm::rotate(rot, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch
    rot = glm::rotate(rot, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Roll
    return glm::normalize(glm::vec3(rot * glm::vec4(forward, 0.0f)));
}
