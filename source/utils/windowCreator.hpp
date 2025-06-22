#pragma once

#include <vector>
#include <stdexcept>
#include "window.hpp"

class WindowCreator {

    std::vector<Window> windowList;

    unsigned int indice = 0;

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

        void Create() {
            Window window;
            window.indice = indice++;
            window.private0 = CREATED;
            WindowInit(&window);
            windowList.push_back(window);
        };

        void ShowAll() {
            for (Window& window : windowList)  {
               WindowShow(&window);
            }
        }

        void ShowByIndice(INDICE indice) { WindowShow(&windowList.at(FoundIndice(indice))); }

        Window GetFist() const {  return windowList.at(0); }
        Window GetLast() const {  return windowList.at(indice); }

        size_t GetSizeList() const { return windowList.size(); }

        Window GetByIndice(INDICE indice) { return windowList.at(FoundIndice(indice)); }
};
