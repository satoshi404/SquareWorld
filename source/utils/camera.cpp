#include "camera.h"
#include <stdexcept>

Camera::Camera(const nlohmann::json& json) {
    if (!json.is_object()) {
        throw std::runtime_error("Camera JSON must be an object");
    }
    if (!json.contains("position") || !json["position"].is_object() ||
        !json["position"].contains("x") || !json["position"].contains("y") || !json["position"].contains("z")) {
        throw std::runtime_error("Camera missing or invalid 'position' (x, y, z)");
    }
    if (!json.contains("target") || !json["target"].is_object() ||
        !json["target"].contains("x") || !json["target"].contains("y") || !json["target"].contains("z")) {
        throw std::runtime_error("Camera missing or invalid 'target' (x, y, z)");
    }
    if (!json.contains("fov") || !json["fov"].is_number()) {
        throw std::runtime_error("Camera missing or invalid 'fov'");
    }
    if (!json.contains("aspect") || !json["aspect"].is_number()) {
        throw std::runtime_error("Camera missing or invalid 'aspect'");
    }
    if (!json.contains("near") || !json["near"].is_number()) {
        throw std::runtime_error("Camera missing or invalid 'near'");
    }
    if (!json.contains("far") || !json["far"].is_number()) {
        throw std::runtime_error("Camera missing or invalid 'far'");
    }

    position = glm::vec3(
        json["position"]["x"].get<float>(),
        json["position"]["y"].get<float>(),
        json["position"]["z"].get<float>()
    );
    target = glm::vec3(
        json["target"]["x"].get<float>(),
        json["target"]["y"].get<float>(),
        json["target"]["z"].get<float>()
    );
    fov = json["fov"].get<float>();
    aspect = json["aspect"].get<float>();
    near = json["near"].get<float>();
    far = json["far"].get<float>();
}

void Camera::setUniforms(GLuint shaderProgram) const {
    glm::mat4 view = glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(fov), aspect, near, far);
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
}

void Camera::updateJSON(nlohmann::json& json) const {
    json["position"] = { {"x", position.x}, {"y", position.y}, {"z", position.z} };
    json["target"] = { {"x", target.x}, {"y", target.y}, {"z", target.z} };
    json["fov"] = fov;
    json["aspect"] = aspect;
    json["near"] = near;
    json["far"] = far;
}