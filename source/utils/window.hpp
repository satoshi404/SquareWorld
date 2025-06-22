#ifndef WINDOW
#define WINDOW

#include "../ventor/platform.hpp"

//////////////////////////////////////////////////////////////////////////////

#if defined(PLATFORM_LINUX)
#include "../ventor/x11.hpp"
#else
#error "PLATFORM NOT SUPPORTED"
#endif

#define INDICE unsigned int 

enum STATUS_CREATOR {
    NOT_CREATED = 0xfb,
    CREATED     = 0xff
};

typedef struct Window 
{

        INDICE indice; // Window indice
        STATUS_CREATOR private0 = NOT_CREATED; // Window created 

        unsigned int initialized = 0; // False
        

    #if defined(PLATFORM_LINUX)
        XDisplay* dpy;
        XWindow window;
        int screen;
    #endif

    #if CONTEXT_OPENGL
        #if defined(PLATFORM_LINUX)
            //GLXContext glContext; // OpenGL context for X11
            //GLXFBConfig fbConfig; // Framebuffer configuration for OpenGL
        #endif
    #endif

};
void WindowInit(Window* window);
void WindowShow(Window* window);
void WindowDestroy(Window* window);

///////////////////////////////////////////////////////////////////////////////
    #endif ///// window.hpp ///////
///////////////////////////////////////////////////////////////////////////////