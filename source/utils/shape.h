
#pragma once
#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

class Shape {
protected:
    std::string type;
    GLuint vao, vbo, ebo;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    glm::vec3 position; // Added
    glm::vec3 scale;    // Added
    glm::vec3 rotation; // Added (Euler angles in degrees)
    glm::vec4 color;    // Added (RGBA)

public:
    Shape(const std::string& type);
    virtual ~Shape();
    virtual void init();
    virtual void draw(GLuint shaderProgram);
    std::string getType() const { return type; }
    static Shape* createShape(const std::string& type);

    // Getters and setters for properties
    glm::vec3 getPosition() const { return position; }
    void setPosition(const glm::vec3& pos) { position = pos; }
    glm::vec3 getScale() const { return scale; }
    void setScale(const glm::vec3& scl) { scale = scl; }
    glm::vec3 getRotation() const { return rotation; }
    void setRotation(const glm::vec3& rot) { rotation = rot; }
    glm::vec4 getColor() const { return color; }
    void setColor(const glm::vec4& col) { color = col; }
};
