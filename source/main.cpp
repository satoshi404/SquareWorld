#include "utils/window.h"
#include "utils/renderer.h" // Added for Renderer definition
#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <imgui.h>
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <iostream>

// Constants
const unsigned int DEF_WINDOW_W = 800;
const unsigned int DEF_WINDOW_H = 600;

int main() {
    std::vector<Window*> windows;
    std::vector<Renderer*> renderers;

    try {
        // Load scene.json
        nlohmann::json scene;
        std::ifstream file("scene.json");
        if (file.is_open()) {
            file >> scene;
            file.close();
        }

        // Create windows from JSON
        if (scene.contains("windows") && scene["windows"].is_array()) {
            for (const auto& win : scene["windows"]) {
                WindowType type = WINDOW_MAIN;
                if (win.contains("type") && win["type"].is_string()) {
                    std::string typeStr = win["type"].get<std::string>();
                    if (typeStr == "WINDOW_DEBUG") type = WINDOW_DEBUG;
                    else if (typeStr == "WINDOW_HIERARCHY") type = WINDOW_HIERARCHY;
                    else if (typeStr == "WINDOW_GUI") type = WINDOW_GUI;
                }

                unsigned int width = win.contains("width") && win["width"].is_number_unsigned()
                    ? win["width"].get<unsigned int>() : DEF_WINDOW_W;
                unsigned int height = win.contains("height") && win["height"].is_number_unsigned()
                    ? win["height"].get<unsigned int>() : DEF_WINDOW_H;

                Window* window = new Window(width, height, nullptr, type);
                window->LoadFromJSON(win);
                window->Show();
                windows.push_back(window);
            }
        } else {
            // Create default window if no JSON or no windows
            Window* window = new Window(DEF_WINDOW_W, DEF_WINDOW_H, nullptr, WINDOW_MAIN);
            window->Show();
            windows.push_back(window);
        }

        // Main loop
        int state = 1;
        SDL_Event event;
        while (state) {
            while (SDL_PollEvent(&event)) {
                ImGui_ImplSDL2_ProcessEvent(&event);
                if (event.type == SDL_QUIT) {
                    state = 0;
                } else if (event.type == SDL_WINDOWEVENT) {
                    if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                        state = 0;
                    } else if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        for (Window* window : windows) {
                            if (SDL_GetWindowID(window->GetWindow()) == event.window.windowID) {
                                int width, height;
                                SDL_GetWindowSize(window->GetWindow(), &width, &height);
                                window->renderer->updateCameraAspect(static_cast<float>(width) / height);
                            }
                        }
                    }
                }
            }

            // Update renderers list
            renderers.clear();
            for (Window* window : windows) {
                if (window->type != WINDOW_HIERARCHY) {
                    renderers.push_back(window->renderer);
                }
            }

            // Draw windows
            for (Window* window : windows) {
                window->Draw(&state, renderers);
            }
        }

        // Cleanup
        for (Window* window : windows) {
            delete window;
        }
        windows.clear();
        renderers.clear();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        SDL_Quit();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        for (Window* window : windows) {
            delete window;
        }
        windows.clear();
        renderers.clear();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        SDL_Quit();
        return 1;
    }

    return 0;
}