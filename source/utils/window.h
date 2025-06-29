#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <nlohmann/json.hpp>
#include <vector>

// Forward declaration of Renderer
class Renderer;

// Define WindowType enum
enum WindowType {
    WINDOW_MAIN,
    WINDOW_DEBUG,
    WINDOW_HIERARCHY,
    WINDOW_GUI
};

class Window {
private:
    SDL_Window* window;
    SDL_GLContext glContext;
    unsigned int width, height;
    float aspect;
    std::string title;
    static int indice;
    static bool g_ImGuiInitialized;

public:
    Renderer* renderer;
    WindowType type;
    Window(unsigned int width, unsigned int height, SDL_GLContext sharedContext = nullptr, WindowType type = WINDOW_MAIN);
    ~Window();
    void SetTitle(const char* title);
    void Show();
    void LoadFromJSON(const nlohmann::json& json);
    void Draw(int* state, std::vector<Renderer*>& allRenderers);
    SDL_Window* GetWindow() const { return window; }
    SDL_GLContext GetGLContext() const { return glContext; } // Added
};
