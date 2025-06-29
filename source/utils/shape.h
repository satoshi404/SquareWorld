#ifndef SHAPE_H
#define SHAPE_H

#include <GL/glew.h>
#include <vector>

struct Vertex {
    float x, y, z; // Position
    float r, g, b; // Color
};

class Shape {
public:
    virtual ~Shape() {}
    virtual void init(GLuint shaderProgram) = 0;
    virtual void render() = 0;
    virtual GLuint getVAO() const = 0;
};

class Triangle : public Shape {
    std::vector<Vertex> vertices;
    GLuint vao, vbo;

public:
    Triangle(const std::vector<Vertex>& verts);
    ~Triangle();
    void init(GLuint shaderProgram) override;
    void render() override;
    GLuint getVAO() const override { return vao; }
};

class Circle : public Shape {
    Vertex center;
    float radius;
    int segments;
    Vertex color;
    GLuint vao, vbo;

public:
    Circle(const Vertex& center, float radius, int segments, const Vertex& color);
    ~Circle();
    void init(GLuint shaderProgram) override;
    void render() override;
    GLuint getVAO() const override { return vao; }
};

class Cube : public Shape {
    Vertex position;
    float size;
    Vertex color;
    GLuint vao, vbo, ebo;

public:
    Cube(const Vertex& pos, float size, const Vertex& color);
    ~Cube();
    void init(GLuint shaderProgram) override;
    void render() override;
    GLuint getVAO() const override { return vao; }
};

#endif // SHAPE_H