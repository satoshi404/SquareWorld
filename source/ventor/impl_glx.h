#ifndef IMGUI_IMPL_GLX_H
#define IMGUI_IMPL_GLX_H

#include <X11/Xlib.h>
#include <GL/glx.h>

bool ImGui_ImplGLX_Init(Display* display, Window window, GLXContext gl_context);
void ImGui_ImplGLX_Shutdown();
void ImGui_ImplGLX_NewFrame();
void ImGui_ImplGLX_ProcessEvent(XEvent* event);

#endif // IMGUI_IMPL_GLX_H