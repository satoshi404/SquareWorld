#pragma once

#include <vector>
#include <stdexcept>
#include "window.h"

class WindowCreator {

    std::vector<MyWindow> windowList;
    INDICE indice_count = 0;

    unsigned int FoundIndice(INDICE indice) {
        for (MyWindow& window : windowList) {
            if (window.indice == indice) return indice;
        }
        throw std::runtime_error("Not found indice in window list");
    }

    public:
        WindowCreator() : windowList() {};
        ~WindowCreator() {
            for (MyWindow& window : windowList) {
                WindowDestroy(&window);
            }          

            windowList.clear();
        }

        void Create(const char* title = "", unsigned int w = 0, unsigned int h = 0) {
            MyWindow window;
            window.private0 = CREATED;
            window.indice = indice_count++;
            WindowInit(&window);
            //WindowSetTitle(&window, title);
            windowList.push_back(window);
        };

        void ShowAll() {
            for (MyWindow& window : windowList)  {
               WindowShow(&window);
            }
        }

        void ShowByIndice(INDICE indice) { WindowShow(&windowList.at(FoundIndice(indice))); }

        MyWindow GetFist() const {  return windowList.at(0); }
        MyWindow GetLast() const {  return windowList.at(windowList.size() - 1); }

        size_t GetSizeList() const { return windowList.size(); }

        MyWindow GetByIndice(INDICE indice) { return windowList.at(FoundIndice(indice)); }
};
