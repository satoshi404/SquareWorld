#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "nlohmann/json.hpp"

struct Vertex {
    float x, y, z; // Position
    float r, g, b; // Color
    float u, v;    // Texture coordinates
};

class Shape {
public:
    virtual ~Shape() {}
    virtual void init(GLuint shaderProgram) = 0;
    virtual void render() = 0;
    virtual std::string getType() const = 0;
    virtual void updateJSON(nlohmann::json& json) const = 0;
    virtual glm::mat4 getModelMatrix() const = 0;
};

class Triangle : public Shape {
public:
    Triangle(const std::vector<Vertex>& vertices);
    void init(GLuint shaderProgram) override;
    void render() override;
    std::string getType() const override { return "triangle"; }
    void updateJSON(nlohmann::json& json) const override;
    glm::mat4 getModelMatrix() const override;

private:
    std::vector<Vertex> vertices;
    GLuint vao, vbo;
};

class Circle : public Shape {
public:
    Circle(const Vertex& center, float radius, int segments, const Vertex& color);
    void init(GLuint shaderProgram) override;
    void render() override;
    std::string getType() const override { return "circle"; }
    void updateJSON(nlohmann::json& json) const override;
    glm::mat4 getModelMatrix() const override;

private:
    Vertex center;
    float radius;
    int segments;
    Vertex color;
    GLuint vao, vbo;
};

class Cube : public Shape {
public:
    Cube(const Vertex& position, float size, const Vertex& color);
    void init(GLuint shaderProgram) override;
    void render() override;
    std::string getType() const override { return "cube"; }
    void updateJSON(nlohmann::json& json) const override;
    glm::mat4 getModelMatrix() const override;

private:
    Vertex position;
    float size;
    Vertex color;
    GLuint vao, vbo, ebo;
};

class Mesh : public Shape {
public:
    Mesh(const std::string& objFile, const std::string& textureFile, const glm::vec3& position, float scale);
    ~Mesh();
    void init(GLuint shaderProgram) override;
    void render() override;
    std::string getType() const override { return "mesh"; }
    void updateJSON(nlohmann::json& json) const override;
    glm::mat4 getModelMatrix() const override;

private:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    glm::vec3 position;
    float scale;
    std::string objFile;
    std::string textureFile;
    GLuint vao, vbo, ebo, texture;
};