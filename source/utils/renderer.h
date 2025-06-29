
#pragma once

#include <vector>
#include <glad/glad.h>
#include "shape.h"
#include "mesh.h"
#include "spotlight.h"
#include "camera.h"
#include "gamecamera.h" // Added
#include "window.h"
#include "nlohmann/json.hpp"
#include <chrono>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <mutex>
#include <queue>

// Forward declaration of Window
class Window;

class Renderer {
private:
    void compileShader(GLenum type, const char* source, GLuint& shader);
    void createShaderProgram(const char* vertexSource, const char* fragmentSource);
    void createDebugShaderProgram();
    GLuint shaderProgram;
    GLuint debugShaderProgram;
    std::vector<Shape*> shapes;
    std::vector<Spotlight*> spotlights;
    std::vector<GameCamera*> gameCameras; // Added
    Camera* camera;
    Shape* selectedShape;
    Spotlight* selectedSpotlight;
    GameCamera* selectedGameCamera; // Added
    mutable std::chrono::high_resolution_clock::time_point lastFrameTime;
    std::mutex shapeMutex;
    std::queue<Shape*> pendingShapes;

public:
    Window* window;
    WindowType type;
    Renderer(const char* vertexShaderSource = nullptr, const char* fragmentShaderSource = nullptr);
    ~Renderer();
    void init();
    void render();
    void loadFromJSON(const nlohmann::json& json);
    void setupImGui();
    void renderImGui(bool isDebugWindow, float fps, std::vector<Renderer*>& allRenderers);
    float getFPS() const;
    std::vector<Shape*>& getShapes() { return shapes; }
    std::vector<Spotlight*>& getSpotlights() { return spotlights; }
    std::vector<GameCamera*>& getGameCameras() { return gameCameras; } // Added
    Shape* getSelectedShape() const { return selectedShape; }
    void setSelectedShape(Shape* shape) { selectedShape = shape; }
    Spotlight* getSelectedSpotlight() const { return selectedSpotlight; }
    void setSelectedSpotlight(Spotlight* light) { selectedSpotlight = light; }
    GameCamera* getSelectedGameCamera() const { return selectedGameCamera; } // Added
    void setSelectedGameCamera(GameCamera* cam) { selectedGameCamera = cam; } // Added
    void updateCameraAspect(float aspect);
    void SetType(WindowType tp) { type = tp; }
    void SetWindow(Window* wm) { window = wm; }
    Window* GetWindow() const { return window; }
    WindowType GetType() const { return type; }
};
