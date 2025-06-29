#pragma once

#include <vector>
#include <stdexcept>
#include "window.h"

class WindowCreator {

    std::vector<Window> windowList;
    INDICE indice_count = 0;

    unsigned int FoundIndice(INDICE indice) {
        for (Window& window : windowList) {
            if (window.indice == indice) return indice;
        }
        throw std::runtime_error("Not found indice in window list");
    }

    public:
        WindowCreator() : windowList() {};
        ~WindowCreator() {
            for (Window& window : windowList) {
                WindowDestroy(&window);
            }          

            windowList.clear();
        }

        void Create(const char* title = "", unsigned int w = 0, unsigned int h = 0) {
            Window window;
            window.private0 = CREATED;
            window.indice = indice_count++;
            WindowInit(&window);
            //WindowSetTitle(&window, title);
            windowList.push_back(window);
        };

        void ShowAll() {
            for (Window& window : windowList)  {
               WindowShow(&window);
            }
        }

        void ShowByIndice(INDICE indice) { WindowShow(&windowList.at(FoundIndice(indice))); }

        Window GetFist() const {  return windowList.at(0); }
        Window GetLast() const {  return windowList.at(windowList.size() - 1); }

        size_t GetSizeList() const { return windowList.size(); }

        Window GetByIndice(INDICE indice) { return windowList.at(FoundIndice(indice)); }
};
