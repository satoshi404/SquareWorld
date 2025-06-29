#ifndef WINDOW
#define WINDOW

#include "../ventor/platform.h"
#include "renderer.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>

#if defined(PLATFORM_LINUX)
#include "../ventor/x11.h"
#elif defined(PLATFORM_WINDOWS)
#error "NOT IMPLEMENTED YET"
#else
#error "PLATFORM NOT SUPPORTED"
#endif


constexpr const char* DEF_WINDOW_T = "Window ";
#define DEF_WINDOW_W  800
#define DEF_WINDOW_H  600


#define INDICE unsigned int 

enum STATUS_CREATOR {
    NOT_CREATED = 0xfb,
    CREATED     = 0xff
};

struct MyWindow 
{
    INDICE indice = 0; // Window indice
    STATUS_CREATOR private0 = NOT_CREATED; // Window created 
    unsigned int initialised = 0; // False
    Renderer* renderer; // Renderer for this window

    #if LINUX_IMPL
    XDisplay* dpy;
    XWindow window;
    XEvent event;
    int screen;
    #endif

    #if CONTEXT_OPENGL
    #if LINUX_IMPL
    GLXContext glContext; // OpenGL context for X11
    GLXFBConfig fbConfig; // Framebuffer configuration for OpenGL
    XVisualInfo *visual;
    Colormap colormap;
    #endif
    #endif
};

void WindowInit(MyWindow* window, unsigned int width = 800, unsigned int height = 600, GLXContext sharedContext = nullptr);
void WindowSetTitle(MyWindow* window, const char* title);
void WindowDraw(MyWindow* window, int *state);
void WindowShow(MyWindow* window);
void WindowDestroy(MyWindow* window);

#endif // window.h