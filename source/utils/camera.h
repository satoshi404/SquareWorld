#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "nlohmann/json.hpp"
#include <GL/glew.h>
#include <GL/gl.h>


class Camera {
public:
    Camera(const nlohmann::json& json);
    void setUniforms(GLuint shaderProgram) const;
    void updateJSON(nlohmann::json& json) const;

private:
    glm::vec3 position;
    glm::vec3 target;
    float fov;
    float aspect;
    float near;
    float far;
};