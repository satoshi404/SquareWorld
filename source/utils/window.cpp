#include "window.h"
#include <stdexcept>
#include <string>
#include "nlohmann/json.hpp"
#include <fstream>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <ventor/impl_glx.h>


// Static ImGui context to be shared across all windows
static bool g_ImGuiInitialized = false;

void WindowInit(MyWindow* window, unsigned int width, unsigned int height, GLXContext sharedContext, WindowType type) {
    if (!window) throw std::runtime_error("Window pointer is null");
    if (window->private0 == NOT_CREATED) throw std::runtime_error("Create Window first");

    window->type = type;
    window->dpy = XOpenDisplay(nullptr);
    if (!window->dpy) throw std::runtime_error("Failed init X11");

    window->screen = DefaultScreen(window->dpy);

    #if CONTEXT_OPENGL
    GLint glxAttributes[] = {
        GLX_RGBA,
        GLX_DEPTH_SIZE, 24,
        GLX_DOUBLEBUFFER,
        None
    };
    
    window->visual = glXChooseVisual(window->dpy, window->screen, glxAttributes);
    if (!window->visual) {
        XCloseDisplay(window->dpy);
        throw std::runtime_error("No suitable visual found");
    }

    window->colormap = XCreateColormap(window->dpy, RootWindow(window->dpy, window->screen), window->visual->visual, AllocNone);

    XSetWindowAttributes windowAttributes;
    windowAttributes.colormap = window->colormap;
    windowAttributes.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

    window->window = XCreateWindow(
        window->dpy, RootWindow(window->dpy, window->screen), 0, 0, width, height, 0,
        window->visual->depth, InputOutput, window->visual->visual,
        CWColormap | CWEventMask, &windowAttributes
    );
    if (!window->window) {
        throw std::runtime_error("Failed create window");
    }

    std::string name = std::string(DEF_WINDOW_T) + std::to_string(window->indice);
    XStoreName(window->dpy, window->window, name.c_str());

    window->glContext = glXCreateContext(window->dpy, window->visual, sharedContext, GL_TRUE);
    if (!window->glContext) {
        XFreeColormap(window->dpy, window->colormap);
        XFree(window->visual);
        XCloseDisplay(window->dpy);
        throw std::runtime_error("Failed to create GLX context");
    }
    if (!glXMakeCurrent(window->dpy, window->window, window->glContext)) {
        glXDestroyContext(window->dpy, window->glContext);
        XFreeColormap(window->dpy, window->colormap);
        XFree(window->visual);
        XCloseDisplay(window->dpy);
        throw std::runtime_error("Failed to make GLX context current");
    }

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        glXDestroyContext(window->dpy, window->glContext);
        XFreeColormap(window->dpy, window->colormap);
        XFree(window->visual);
        XCloseDisplay(window->dpy);
        throw std::runtime_error("Failed to initialize GLEW: " + std::string((const char*)glewGetErrorString(err)));
    }

    window->renderer = new Renderer(nullptr, nullptr);

    // Initialize ImGui context and backends only once
    if (!g_ImGuiInitialized) {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui_ImplGLX_Init(window->dpy, window->window, window->glContext);
        ImGui_ImplOpenGL3_Init("#version 330");
        g_ImGuiInitialized = true;
    }
    #endif

    window->initialised = 1;
}

void WindowSetTitle(MyWindow* window, const char* title) { 
    if (!window->initialised) throw std::runtime_error("Window not initialised");
    XStoreName(window->dpy, window->window, title);
}

void WindowShow(MyWindow* window) {
    if (!window->initialised) throw std::runtime_error("Window not initialised");
    if (window->private0 == NOT_CREATED) throw std::runtime_error("Create Window first");
    XMapWindow(window->dpy, window->window);
    XFlush(window->dpy);
}

void WindowDraw(MyWindow* window, int *state, std::vector<Renderer*>& allRenderers) {
    #if CONTEXT_OPENGL
    if (!glXMakeCurrent(window->dpy, window->window, window->glContext)) {
        throw std::runtime_error("Failed to make GLX context current");
    }
    glViewport(0, 0, 800, 600);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    window->renderer->render();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLX_NewFrame();
    window->renderer->setupImGui();
    window->renderer->renderImGui(window->type == WINDOW_DEBUG, window->renderer->getFPS(), allRenderers);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glXSwapBuffers(window->dpy, window->window);
    #endif

    while (XPending(window->dpy)) {
        XNextEvent(window->dpy, &window->event);
        ImGui_ImplGLX_ProcessEvent(&window->event);
        if (window->event.type == KeyPress) {
            KeySym keysym = XLookupKeysym(&window->event.xkey, 0);
            if (keysym == XK_Escape) {
                *state = 0;
            }
        }
    }
}

void WindowDestroy(MyWindow* window) {
    if (!window->initialised) throw std::runtime_error("Window not initialised");
    if (window->private0 == NOT_CREATED) throw std::runtime_error("Create Window first");

    #if CONTEXT_OPENGL
    delete window->renderer;
    glXMakeCurrent(window->dpy, None, NULL);
    glXDestroyContext(window->dpy, window->glContext);
    XFreeColormap(window->dpy, window->colormap);
    XFree(window->visual);
    #endif
    XDestroyWindow(window->dpy, window->window);
    XCloseDisplay(window->dpy);
}