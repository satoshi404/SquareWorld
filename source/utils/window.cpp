#include "window.h"

#include <stdexcept>
#include <string>

constexpr const char* DEF_WINDOW_T = "Window ";
#define DEF_WINDOW_W  800
#define DEF_WINDOW_H  800

void WindowInit(MyWindow* window) {

    
    if (window->private0 == NOT_CREATED) throw std::runtime_error("Create Window first");

    window->dpy = XOpenDisplay(nullptr);
    if (!window->dpy) throw std::runtime_error("Failed init X11");

    window->window = XCreateSimpleWindow(
        window->dpy, XDefaultRootWindow(window->dpy), 
        10, 10, DEF_WINDOW_W, DEF_WINDOW_H, 1, 
        0x0, 0xffffff
    );
    if (!window->window) {
         throw std::runtime_error("Failed create window");
    }

    std::string name = std::string(DEF_WINDOW_T) + std::to_string(window->indice);
    XStoreName(window->dpy, window->window, name.c_str() );

    window->initialised = 1; // True
}

void WindowSetTitle(MyWindow* window, const char* title) { 
    XStoreName(window->dpy, window->window, title);
}

void WindowShow(MyWindow* window) {
    if (window->initialised == 0) throw std::runtime_error("Window not initialised");
    if (window->private0 == NOT_CREATED) throw std::runtime_error("Create Window first");
    XMapWindow(window->dpy, window->window);
    XFlush(window->dpy);
}

void WindowDestroy(MyWindow* window) {
    if (window->initialised == 0) throw std::runtime_error("Window not initialised");
    if (window->private0 == NOT_CREATED) throw std::runtime_error("Create Window first");
    XDestroyWindow(window->dpy, window->window);
    XCloseDisplay(window->dpy);
}