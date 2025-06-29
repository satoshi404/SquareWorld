
#include "mesh.h"
#include <tinyobjloader/tiny_obj_loader.h>
#include <iostream>
#include <fstream>

Mesh::Mesh(const std::string& path) : Shape("Mesh"), objPath(path) {}

void Mesh::init() {
    if (!glad_glGenVertexArrays) {
        throw std::runtime_error("OpenGL not initialized");
    }

    std::ifstream file(objPath);
    if (!file.good()) {
        throw std::runtime_error("Cannot open .obj file: " + objPath);
    }
    file.close();

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objPath.c_str())) {
        throw std::runtime_error("Failed to load .obj file: " + objPath + "\n" + warn + err);
    }

    vertices.clear();
    indices.clear();

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);
            indices.push_back(indices.size());
        }
    }

    if (vertices.empty() || indices.empty()) {
        throw std::runtime_error("No vertices or indices loaded from .obj file: " + objPath);
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
