
#include "shape.h"
#include <glm/glm.hpp>
#include <stdexcept>
#include <iostream>

Shape::Shape(const std::string& type) : type(type), position(0.0f), scale(1.0f), rotation(0.0f), color(1.0f), vao(0), vbo(0), ebo(0) {}

Shape::~Shape() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void Shape::init() {
    if (!glad_glGenVertexArrays) {
        throw std::runtime_error("OpenGL not initialized");
    }

    if (type == "Cube") {
        vertices = {
            -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f
        };
        indices = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            0, 4, 7, 7, 3, 0,
            1, 5, 6, 6, 2, 1,
            3, 2, 6, 6, 7, 3,
            0, 1, 5, 5, 4, 0
        };
    } else if (type == "Circle") {
        const int segments = 32;
        vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
        for (int i = 0; i <= segments; ++i) {
            float theta = 2.0f * 3.14159f * float(i) / float(segments);
            vertices.push_back(cos(theta) * 0.5f);
            vertices.push_back(sin(theta) * 0.5f);
            vertices.push_back(0.0f);
            if (i < segments) {
                indices.push_back(0);
                indices.push_back(i + 1);
                indices.push_back(i + 2);
            }
        }
    } else if (type == "Triangle") {
        vertices = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };
        indices = {0, 1, 2};
    } else {
        throw std::runtime_error("Unsupported shape type: " + type);
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Shape::draw(GLuint shaderProgram) {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Shape* Shape::createShape(const std::string& type) {
    if (type == "Circle" || type == "Cube" || type == "Triangle") {
        return new Shape(type);
    } else if (type == "Mesh") {
        return nullptr; // Mesh requires objPath, handled in Renderer
    }
    std::cerr << "Unknown shape type: " << type << std::endl;
    return nullptr;
}

