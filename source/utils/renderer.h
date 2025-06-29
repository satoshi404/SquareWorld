#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <vector>
#include <string>
#include "shape.h"
#include "nlohmann/json.hpp"

class Renderer {
    GLuint shaderProgram;
    std::vector<Shape*> shapes;

    void compileShader(GLenum type, const char* source, GLuint& shader);
    void createShaderProgram(const char* vertexSource, const char* fragmentSource);

public:
    Renderer(const char* vertexShaderSource = nullptr, const char* fragmentShaderSource = nullptr);
    ~Renderer();

    void init();
    void render();
    void loadFromJSON(const nlohmann::json& json);
    GLuint getShaderProgram() const { return shaderProgram; }
};

#endif // RENDERER_H