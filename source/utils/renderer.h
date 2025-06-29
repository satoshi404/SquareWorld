#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <vector>
#include <string>
#include "shape.h"
#include "camera.h"
#include "nlohmann/json.hpp"
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <ventor/impl_glx.h>
#include <chrono>

class Renderer {
    GLuint shaderProgram;
    std::vector<Shape*> shapes;
    Camera* camera;          
    Shape* selectedShape;
    mutable float lastFrameTime;

    void compileShader(GLenum type, const char* source, GLuint& shader);
    void createShaderProgram(const char* vertexSource, const char* fragmentSource);

public:
    Renderer(const char* vertexShaderSource = nullptr, const char* fragmentShaderSource = nullptr);
    ~Renderer();

    void init();
    void render();
    void loadFromJSON(const nlohmann::json& json);
    GLuint getShaderProgram() const { return shaderProgram; }
    void setupImGui();
    void renderImGui(bool isDebugWindow, float fps, std::vector<Renderer*>& allRenderers);
    std::vector<Shape*>& getShapes() { return shapes; }
    Camera* getCamera() { return camera; }
    void setSelectedShape(Shape* shape) { selectedShape = shape; }
    float getFPS() const;
};

#endif // RENDERER_H