#ifndef REGISTER_H
#define REGISTER_H

#include <sol/sol.hpp>
#include <stdexcept>
#include "bird.h"

class Register {
    sol::state lua;

public:
    Register() {
        lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table);
    }

    // Register C++ types with Lua
    void RegisterTypes() {
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
    }

    // Expose a Bird instance to Lua
    void RegisterEntity(Bird* bird, const std::string& name = "bird") {
        lua[name] = bird;
    }

    // Load and execute a Lua script
    void LoadScript(const std::string& path) {
        sol::protected_function_result result = lua.script_file(path, sol::script_pass_on_error);
        if (!result.valid()) {
            sol::error err = result;
            throw std::runtime_error("Erro ao executar " + path + ": " + err.what());
        }
    }

    // Get the update function for the game loop
    sol::protected_function GetUpdateFunction() {
        sol::protected_function update = lua["update"];
        if (!update.valid()) {
            throw std::runtime_error("Lua function 'update' not found");
        }
        return update;
    }

    // Set a global Lua variable (e.g., space_pressed)
    void SetGlobal(const std::string& name, bool value) {
        lua[name] = value;
    }

    // Get the Lua state (for advanced use)
    sol::state& GetLuaState() {
        return lua;
    }
};

#endif