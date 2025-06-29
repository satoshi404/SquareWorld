
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    glm::vec3 position; // Camera position
    glm::vec3 rotation; // Euler angles (degrees: pitch, yaw, roll)
    float fov;          // Field of view (degrees)
    float aspect;       // Aspect ratio
    float nearClip;     // Near clipping plane
    float farClip;      // Far clipping plane

public:
    Camera();
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    void setAspect(float aspect);
    // Getters and setters for GUI editing
    glm::vec3 getPosition() const { return position; }
    void setPosition(const glm::vec3& pos) { position = pos; }
    glm::vec3 getRotation() const { return rotation; }
    void setRotation(const glm::vec3& rot) { rotation = rot; }
    float getFov() const { return fov; }
    void setFov(float f) { fov = f; }
};
