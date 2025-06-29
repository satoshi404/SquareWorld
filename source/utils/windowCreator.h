#pragma once

#include <vector>
#include <stdexcept>
#include <fstream>
#include <unistd.h>
#include "window.h"
#include <iostream>
#include <nlohmann/json.hpp>

class WindowCreator {
    std::vector<MyWindow> windowList;
    INDICE indice_count = 0;
    int state = 1; // true

    unsigned int FoundIndice(INDICE indice) {
        for (size_t i = 0; i < windowList.size(); ++i) {
            if (windowList[i].indice == indice) return i;
        }
        throw std::runtime_error("Not found indice in window list");
    }

public:
    WindowCreator() : windowList() {}
    ~WindowCreator() {
        for (MyWindow& window : windowList) {
            WindowDestroy(&window);
        }
        windowList.clear();
    }

    void Create(WindowType type, const char* title = "", unsigned int w = 0, unsigned int h = 0) {
        MyWindow window;
        window.private0 = CREATED;
        window.indice = indice_count++;
        GLXContext sharedContext = windowList.empty() ? nullptr : windowList[0].glContext;
        WindowInit(&window, w ? w : DEF_WINDOW_W, h ? h : DEF_WINDOW_H, sharedContext, type);
        windowList.push_back(window);
        WindowShow(&window);
        if (title && *title) {
            WindowSetTitle(&window, title);
        }
    }

    void LoadFromJSON(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open JSON file: " + filename);
        }
        nlohmann::json json;
        file >> json;

        std::cerr << "JSON: " << json.dump(2) << std::endl;

        if (!json.contains("windows") || !json["windows"].is_array()) {
            throw std::runtime_error("JSON missing 'windows' array");
        }

        for (const auto& win : json["windows"]) {
            if (!win.contains("type") || !win["type"].is_string()) {
                throw std::runtime_error("Window missing 'type' field");
            }
            std::string typeStr = win["type"].get<std::string>();
            WindowType type;
            if (typeStr == "WINDOW_MAIN") type = WINDOW_MAIN;
            else if (typeStr == "WINDOW_DEBUG") type = WINDOW_DEBUG;
            else if (typeStr == "WINDOW_GUI") type = WINDOW_GUI;
            else if (typeStr == "WINDOW_HIERARCHY") type = WINDOW_HIERARCHY;
            else throw std::runtime_error("Invalid window type: " + typeStr);

            std::string title = win.contains("title") && win["title"].is_string() ? win["title"].get<std::string>() : "";
            unsigned int width = win.contains("width") && win["width"].is_number_unsigned() ? win["width"].get<unsigned int>() : DEF_WINDOW_W;
            unsigned int height = win.contains("height") && win["height"].is_number_unsigned() ? win["height"].get<unsigned int>() : DEF_WINDOW_H;

            Create(type, title.c_str(), width, height);

            MyWindow& window = windowList.back();
            if (!glXMakeCurrent(window.dpy, window.window, window.glContext)) {
                throw std::runtime_error("Failed to make GLX context current");
            }
            if (type != WINDOW_HIERARCHY) {
                if (win.contains("shapes") && win["shapes"].is_array()) {
                    window.renderer->loadFromJSON(win);
                    window.renderer->init();
                }
            }
        }
    }

    void DrawAll() {
        std::vector<Renderer*> renderers;
        for (auto& window : windowList) {
            if (window.type != WINDOW_HIERARCHY) {
                renderers.push_back(window.renderer);
            }
        }
        while (state) {
            for (MyWindow& window : windowList) {
                WindowDraw(&window, &state, renderers);
            }
            usleep(10000); // 10ms to avoid high CPU usage
        }
    }

    void ShowByIndice(INDICE indice) { WindowShow(&windowList.at(FoundIndice(indice))); }

    MyWindow GetFirst() const { return windowList[0]; }
    MyWindow GetLast() const { return windowList.at(windowList.size() - 1); }

    size_t GetSizeList() const { return windowList.size(); }

    std::vector<MyWindow> GetListWindow() const { return windowList; }

    MyWindow GetByIndice(INDICE indice) { return windowList.at(FoundIndice(indice)); }
};