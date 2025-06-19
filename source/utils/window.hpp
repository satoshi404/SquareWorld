#ifndef WINDOW
#define WINDOW

#include "../ventor/platform.hpp"

//////////////////////////////////////////////////////////////////////////////

#if defined(PLATFORM_LINUX)
#include "../ventor/x11.hpp"
#else
#error "PLATFORM NOT SUPPORTED"
#endif

typedef struct Window 
{

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

    void (*Destroy)(struct Window* self);

};





///////////////////////////////////////////////////////////////////////////////
    #endif ///// window.hpp ///////
///////////////////////////////////////////////////////////////////////////////