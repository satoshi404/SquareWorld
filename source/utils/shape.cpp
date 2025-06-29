#include "shape.h"
#include <cmath>

Triangle::Triangle(const std::vector<Vertex>& verts) : vertices(verts), vao(0), vbo(0) {}

Triangle::~Triangle() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void Triangle::init(GLuint shaderProgram) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float))); // Color
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Triangle::render() {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

Circle::Circle(const Vertex& center, float radius, int segments, const Vertex& color)
    : center(center), radius(radius), segments(segments), color(color), vao(0), vbo(0) {}

Circle::~Circle() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void Circle::init(GLuint shaderProgram) {
    std::vector<Vertex> vertices;
    for (int i = 0; i < segments; ++i) {
        float theta = 2.0f * M_PI * i / segments;
        vertices.push_back({center.x + radius * std::cos(theta), center.y + radius * std::sin(theta), 0.0f,
                            color.r, color.g, color.b});
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float))); // Color
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Circle::render() {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments);
    glBindVertexArray(0);
}

Cube::Cube(const Vertex& pos, float size, const Vertex& color)
    : position(pos), size(size), color(color), vao(0), vbo(0), ebo(0) {}

Cube::~Cube() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void Cube::init(GLuint shaderProgram) {
    float s = size / 2.0f;
    std::vector<Vertex> vertices = {
        // Front face
        {position.x - s, position.y - s, position.z + s, color.r, color.g, color.b},
        {position.x + s, position.y - s, position.z + s, color.r, color.g, color.b},
        {position.x + s, position.y + s, position.z + s, color.r, color.g, color.b},
        {position.x - s, position.y + s, position.z + s, color.r, color.g, color.b},
        // Back face
        {position.x - s, position.y - s, position.z - s, color.r, color.g, color.b},
        {position.x + s, position.y - s, position.z - s, color.r, color.g, color.b},
        {position.x + s, position.y + s, position.z - s, color.r, color.g, color.b},
        {position.x - s, position.y + s, position.z - s, color.r, color.g, color.b}
    };

    std::vector<GLuint> indices = {
        0, 1, 2, 2, 3, 0, // Front
        1, 5, 6, 6, 2, 1, // Right
        5, 4, 7, 7, 6, 5, // Back
        4, 0, 3, 3, 7, 4, // Left
        3, 2, 6, 6, 7, 3, // Top
        4, 5, 1, 1, 0, 4  // Bottom
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float))); // Color
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Cube::render() {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}