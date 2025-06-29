#pragma once

#include <vector>
#include <GL/glew.h>
#include "shape.h"
#include "camera.h"
#include "nlohmann/json.hpp"
#include <chrono>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>

class Renderer {
public:
    Renderer(const char* vertexShaderSource = nullptr, const char* fragmentShaderSource = nullptr);
    ~Renderer();
    void init();
    void render();
    void loadFromJSON(const nlohmann::json& json);
    void setupImGui();
    void renderImGui(bool isDebugWindow, float fps, std::vector<Renderer*>& allRenderers);
    float getFPS() const;
    std::vector<Shape*>& getShapes() { return shapes; }
    Shape* getSelectedShape() const { return selectedShape; }
    void setSelectedShape(Shape* shape) { selectedShape = shape; }

private:
    void compileShader(GLenum type, const char* source, GLuint& shader);
    void createShaderProgram(const char* vertexSource, const char* fragmentSource);
    GLuint shaderProgram;
    std::vector<Shape*> shapes;
    Camera* camera;
    Shape* selectedShape;
    float lastFrameTime;
};