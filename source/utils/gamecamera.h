#pragma once
#include <glm/glm.hpp>
#include <string>

class GameCamera {
private:
    std::string name;
    glm::vec3 position;
    glm::vec3 rotation; // Euler angles in degrees (pitch, yaw, roll)
    float fov;

public:
    GameCamera(const std::string& name);
    std::string getName() const { return name; }
    glm::vec3 getPosition() const { return position; }
    void setPosition(const glm::vec3& pos) { position = pos; }
    glm::vec3 getRotation() const { return rotation; }
    void setRotation(const glm::vec3& rot) { rotation = rot; }
    float getFov() const { return fov; }
    void setFov(float f) { fov = f; }
    glm::vec3 getForward() const;
};
