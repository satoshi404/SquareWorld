#include <imgui/imgui.h>
#include "impl_glx.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <chrono>

static Display* g_Display;
static Window g_Window;

bool ImGui_ImplGLX_Init(Display* display, Window window, GLXContext gl_context) {
    g_Display = display;
    g_Window = window;
    ImGuiIO& io = ImGui::GetIO();
    io.BackendPlatformName = "imgui_impl_glx";
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    return true;
}

void ImGui_ImplGLX_Shutdown() {
}

void ImGui_ImplGLX_NewFrame() {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(800.0f, 600.0f); // Should query actual window size
    static double lastTime = 0.0;
    double currentTime = std::chrono::duration_cast<std::chrono::duration<double>>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    io.DeltaTime = lastTime > 0.0 ? (float)(currentTime - lastTime) : (float)(1.0f / 60.0f);
    lastTime = currentTime;
}

static ImGuiKey ImGui_ImplGLX_KeySymToImGuiKey(KeySym sym) {
    switch (sym) {
        case XK_Return: return ImGuiKey_Enter;
        case XK_Escape: return ImGuiKey_Escape;
        case XK_space: return ImGuiKey_Space;
        case XK_Tab: return ImGuiKey_Tab;
        case XK_BackSpace: return ImGuiKey_Backspace;
        case XK_a: case XK_A: return ImGuiKey_A;
        case XK_b: case XK_B: return ImGuiKey_B;
        // Add more key mappings as needed
        default: return ImGuiKey_None;
    }
}

void ImGui_ImplGLX_ProcessEvent(XEvent* event) {
    ImGuiIO& io = ImGui::GetIO();
    if (event->type == ButtonPress) {
        io.AddMouseButtonEvent(event->xbutton.button - 1, true);
    } else if (event->type == ButtonRelease) {
        io.AddMouseButtonEvent(event->xbutton.button - 1, false);
    } else if (event->type == MotionNotify) {
        io.AddMousePosEvent((float)event->xmotion.x, (float)event->xmotion.y);
    } else if (event->type == KeyPress) {
        KeySym keysym = XLookupKeysym(&event->xkey, 0);
        ImGuiKey key = ImGui_ImplGLX_KeySymToImGuiKey(keysym);
        if (key != ImGuiKey_None) {
            io.AddKeyEvent(key, true);
        }
        char buffer[32];
        int len = XLookupString(&event->xkey, buffer, sizeof(buffer), nullptr, nullptr);
        if (len > 0) {
            io.AddInputCharactersUTF8(buffer);
        }
    } else if (event->type == KeyRelease) {
        KeySym keysym = XLookupKeysym(&event->xkey, 0);
        ImGuiKey key = ImGui_ImplGLX_KeySymToImGuiKey(keysym);
        if (key != ImGuiKey_None) {
            io.AddKeyEvent(key, false);
        }
    }
}