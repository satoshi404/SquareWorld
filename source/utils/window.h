#ifndef WINDOW
#define WINDOW

#include "../ventor/platform.h"

#include <GL/gl.h>
#include <GL/glx.h>

//////////////////////////////////////////////////////////////////////////////

#if defined(PLATFORM_LINUX)
#include "../ventor/x11.h"
#elif defined(PLATFORM_WINDOWS)
#error "NOT IMPLEMENTED YET"
#else
#error "PLATFORM NOT SUPPORTED"
#endif

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

    #if LINUX_IMPL
        XDisplay* dpy;
        XWindow window;
        int screen;
    #endif

    #if CONTEXT_OPENGL
        #if LINUX_IMPL
            GLXContext glContext; // OpenGL context for X11
            GLXFBConfig fbConfig; // Framebuffer configuration for OpenGL
        #endif
    #endif

};


void WindowInit(MyWindow* window);
void WindowSetTitle(MyWindow* window, const char* title);
void WindowShow(MyWindow* window);
void WindowDestroy(MyWindow* window);

///////////////////////////////////////////////////////////////////////////////
    #endif ///// window.hpp ///////
///////////////////////////////////////////////////////////////////////////////