#include "window.hpp"

#include <stdexcept>

#define DEF_WINDOW_T  "Window title"
#define DEF_WINDOW_W  800
#define DEF_WINDOW_H  800

void WindowInit(Window* window) {

    if (window->private0 == NOT_CREATED) throw std::runtime_error("Window not add in manager first");

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

    XStoreName(window->dpy, window->window, DEF_WINDOW_T);

    window->initialized = 1; // True

}

void WindowShow(Window* window) {
    if (window->initialized == 0) throw std::runtime_error("Window not initialized");
    XMapWindow(window->dpy, window->window);
    XFlush(window->dpy);
}

void WindowDestroy(Window* window) {
    if (window->initialized == 0) throw std::runtime_error("Window not initialized");
    XDestroyWindow(window->dpy, window->window);
    XCloseDisplay(window->dpy);
}