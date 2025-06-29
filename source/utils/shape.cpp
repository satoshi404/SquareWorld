#include "shape.h"
#include <cmath>
#include <tinyobjloader/tiny_obj_loader.h>
#include <stb/stb_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>
#include <vector>

Triangle::Triangle(const std::vector<Vertex>& verts) : vertices(verts) {}

void Triangle::init(GLuint shaderProgram) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void Triangle::render() {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindVertexArray(0);
}

void Triangle::updateJSON(nlohmann::json& json) const {
    json["type"] = "triangle";
    json["vertices"] = nlohmann::json::array();
    for (const auto& v : vertices) {
        json["vertices"].push_back({
            {"x", v.x}, {"y", v.y}, {"z", v.z},
            {"r", v.r}, {"g", v.g}, {"b", v.b}
        });
    }
}

glm::mat4 Triangle::getModelMatrix() const {
    return glm::mat4(1.0f);
}

Circle::Circle(const Vertex& c, float r, int s, const Vertex& col)
    : center(c), radius(r), segments(s), color(col) {}

void Circle::init(GLuint shaderProgram) {
    std::vector<Vertex> vertices;
    vertices.push_back(center);
    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(segments);
        vertices.push_back((Vertex) {
            center.x + radius * cos(theta),
            center.y + radius * sin(theta),
            center.z,
            color.r, color.g, color.b,
            0.0f, 0.0f
        });
    }
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void Circle::render() {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
    glBindVertexArray(0);
}

void Circle::updateJSON(nlohmann::json& json) const {
    json["type"] = "circle";
    json["center"] = { {"x", center.x}, {"y", center.y}, {"z", center.z} };
    json["radius"] = radius;
    json["color"] = { {"r", color.r}, {"g", color.g}, {"b", color.b} };
    json["segments"] = segments;
}

glm::mat4 Circle::getModelMatrix() const {
    return glm::mat4(1.0f);
}

Cube::Cube(const Vertex& pos, float s, const Vertex& col)
    : position(pos), size(s), color(col) {}

void Cube::init(GLuint shaderProgram) {
    std::vector<Vertex> vertices = {
        {-size, -size, -size, color.r, color.g, color.b, 0.0f, 0.0f},
        { size, -size, -size, color.r, color.g, color.b, 1.0f, 0.0f},
        { size,  size, -size, color.r, color.g, color.b, 1.0f, 1.0f},
        {-size,  size, -size, color.r, color.g, color.b, 0.0f, 1.0f},
        {-size, -size,  size, color.r, color.g, color.b, 0.0f, 0.0f},
        { size, -size,  size, color.r, color.g, color.b, 1.0f, 0.0f},
        { size,  size,  size, color.r, color.g, color.b, 1.0f, 1.0f},
        {-size,  size,  size, color.r, color.g, color.b, 0.0f, 1.0f}
    };
    std::vector<GLuint> indices = {
        0, 1, 2, 2, 3, 0, // front
        1, 5, 6, 6, 2, 1, // right
        5, 4, 7, 7, 6, 5, // back
        4, 0, 3, 3, 7, 4, // left
        3, 2, 6, 6, 7, 3, // top
        4, 5, 1, 1, 0, 4  // bottom
    };
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void Cube::render() {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Cube::updateJSON(nlohmann::json& json) const {
    json["type"] = "cube";
    json["position"] = { {"x", position.x}, {"y", position.y}, {"z", position.z} };
    json["size"] = size;
    json["color"] = { {"r", color.r}, {"g", color.g}, {"b", color.b} };
}

glm::mat4 Cube::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
    return model;
}

Mesh::Mesh(const std::string& obj, const std::string& tex, const glm::vec3& pos, float s)
    : objFile(obj), textureFile(tex), position(pos), scale(s) {}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteTextures(1, &texture);
}

void Mesh::init(GLuint shaderProgram) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objFile.c_str())) {
        throw std::runtime_error("Failed to load OBJ file: " + err);
    }
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex;
            vertex.x = attrib.vertices[3 * index.vertex_index + 0] * scale;
            vertex.y = attrib.vertices[3 * index.vertex_index + 1] * scale;
            vertex.z = attrib.vertices[3 * index.vertex_index + 2] * scale;
            vertex.r = vertex.g = vertex.b = 1.0f;
            if (index.texcoord_index >= 0) {
                vertex.u = attrib.texcoords[2 * index.texcoord_index + 0];
                vertex.v = attrib.texcoords[2 * index.texcoord_index + 1];
            } else {
                vertex.u = vertex.v = 0.0f;
            }
            vertices.push_back(vertex);
            indices.push_back(indices.size());
        }
    }
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(textureFile.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = nrChannels == 3 ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        throw std::runtime_error("Failed to load texture: " + textureFile);
    }
    stbi_image_free(data);
    glBindVertexArray(0);
}

void Mesh::render() {
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::updateJSON(nlohmann::json& json) const {
    json["type"] = "mesh";
    json["obj_file"] = objFile;
    json["texture_file"] = textureFile;
    json["position"] = { {"x", position.x}, {"y", position.y}, {"z", position.z} };
    json["scale"] = scale;
}

glm::mat4 Mesh::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(scale));
    return model;
}