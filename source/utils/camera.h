#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    glm::vec3 position;
    glm::vec3 rotation; // Euler angles in degrees (pitch, yaw, roll)
    float fov;
    float aspect;

public:
    Camera();
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::vec3 getPosition() const { return position; }
    void setPosition(const glm::vec3& pos) { position = pos; }
    glm::vec3 getRotation() const { return rotation; }
    void setRotation(const glm::vec3& rot) { rotation = rot; }
    float getFov() const { return fov; }
    void setFov(float f) { fov = f; }
    void setAspect(float a) { aspect = a; }
    glm::vec3 getForward() const; // Added
};
