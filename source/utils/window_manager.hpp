#pragma once

#include <vector>

#define RIDE_LISTWINDOW(interator, size) for (interator; interator < size; interator++)       

#include "window.hpp"

class WindowManager {

    std::vector<Window> windowList;

    void windowFree() {
        int i = 0;
        size_t size = windowList.size();
        RIDE_LISTWINDOW(i, size) {
            Window currentWindow = windowList[i];
            w
        }
    }

    public:
        WindowManager() : windowList({0}) {};
        ~WindowManager() {
            windowFree();          
        }
        void addWindow(Window window);
};
