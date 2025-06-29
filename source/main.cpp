#include "utils/windowCreator.h"
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <ventor/impl_glx.h>
#include <iostream>

int main() {
    WindowCreator creator;
    
    try {
        creator.LoadFromJSON("scene.json");
        creator.DrawAll();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLX_Shutdown();
    ImGui::DestroyContext();

    return 0;
}