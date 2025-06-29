#pragma once

#include <vector>
#include <stdexcept>
#include <fstream>
#include <unistd.h>
#include "window.h"
#include <nlohmann/json.hpp>

enum WindowType {
    WINDOW_MAIN,
    WINDOW_DEBUG,
    WINDOW_GUI
};

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
        WindowInit(&window, w ? w : DEF_WINDOW_W, h ? h : DEF_WINDOW_H, sharedContext);
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

        for (const auto& win : json["windows"]) {
            std::string typeStr = win["type"].get<std::string>();
            WindowType type;
            if (typeStr == "WINDOW_MAIN") type = WINDOW_MAIN;
            else if (typeStr == "WINDOW_DEBUG") type = WINDOW_DEBUG;
            else if (typeStr == "WINDOW_GUI") type = WINDOW_GUI;
            else throw std::runtime_error("Invalid window type: " + typeStr);

            Create(type, win["title"].get<std::string>().c_str(),
                   win["width"].get<unsigned int>(), win["height"].get<unsigned int>());

            MyWindow& window = windowList.back();
            if (!glXMakeCurrent(window.dpy, window.window, window.glContext)) {
                throw std::runtime_error("Failed to make GLX context current");
            }
            window.renderer->loadFromJSON(win);
            window.renderer->init();
        }
    }

    void DrawAll() {
        while (state) {
            for (MyWindow& window : windowList) {
                WindowDraw(&window, &state);
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