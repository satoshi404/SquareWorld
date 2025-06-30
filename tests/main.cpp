#include <sol/sol.hpp>
#include <iostream>
#include <render.h>
#include <bird.h>

int main(int argc, char* argv[]) {
    try {
        // Initialize Lua
        sol::state lua;
        lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table);

        // Register Vec2<float>
        lua.new_usertype<Vec2<float>>("Vec2",
            sol::constructors<Vec2<float>(), Vec2<float>(float, float)>(),
            "x", &Vec2<float>::x,
            "y", &Vec2<float>::y,
            "__tostring", [](const Vec2<float>& v) {
                return "Vec2(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
            }
        );

        // Register Bird
        lua.new_usertype<Bird>("Bird",
            sol::constructors<Bird()>(),
            "new", sol::constructors<Bird()>(),
            "SetPosition", &Bird::SetPosition,
            "SetSize", &Bird::SetSize,
            "SetVelocity", &Bird::SetVelocity,
            "Jump", &Bird::Jump,
            "GetPosition", &Bird::GetPosition,
            "GetSize", &Bird::GetSize,
            "GetVelocity", &Bird::GetVelocity,
            "texture_source", sol::property(&Bird::GetTextureSource, &Bird::SetTextureSource),
            "__tostring", [](const Bird& b) {
                auto pos = b.GetPosition();
                return "Bird at (" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ")";
            }
        );

        // Create Bird instance and expose to Lua
        Bird bird;
        lua["bird"] = &bird;

        // Load Lua script
        sol::protected_function_result result = lua.script_file("../scripts/bird.lua", sol::script_pass_on_error);
        if (!result.valid()) {
            sol::error err = result;
            std::cerr << "Erro ao executar scripts/bird.lua: " << err.what() << std::endl;
            return 1;
        }

        // Initialize Render
        Render renderer;
        renderer.Init();
        renderer.AddEntity(&bird);

        // Run game loop
        renderer.MainLoop(lua);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}