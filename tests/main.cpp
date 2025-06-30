#include <iostream>
#include <render.h>
#include <register.h>
#include <bird.h>

int main(int argc, char* argv[]) {
    try {
        // Initialize Lua and register types
        Register reg;
        reg.RegisterTypes();

        // Create Bird and register with Lua
        Bird bird;
        reg.RegisterEntity(&bird);

        // Load Lua script
        reg.LoadScript("../scripts/bird.lua");

        // Initialize Render
        Render renderer;
        renderer.Init();
        renderer.AddEntity(&bird);

        // Run game loop with Lua state
        renderer.MainLoop(reg.GetLuaState());
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}