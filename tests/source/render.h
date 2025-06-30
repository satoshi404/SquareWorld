#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include <vector>
#include <stdexcept>
#include <sol/sol.hpp>
#include "bird.h"

#define WINDOW_W 800
#define WINDOW_H 600
#define WINDOW_T "Flappy Bird"

enum STATUS_LOOP {
    STOPED = 0,
    RUNNING,
};

class Render {
    SDL_Window* window;
    SDL_Renderer* render;
    std::vector<Bird*> entities; // Store pointers to Bird entities
    STATUS_LOOP status;
    SDL_Event event;
    const float dt = 1.0f / 60.0f; // 60 FPS

public:
    Render() : window(nullptr), render(nullptr), entities(), status(STOPED) {}
    ~Render() {
        if (render) SDL_DestroyRenderer(render);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void Init() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            throw std::runtime_error("SDL_Init failed: " + std::string(SDL_GetError()));
        }

        window = SDL_CreateWindow(
            WINDOW_T,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            WINDOW_W, WINDOW_H,
            SDL_WINDOW_SHOWN
        );
        if (!window) {
            SDL_Quit();
            throw std::runtime_error("SDL_CreateWindow failed: " + std::string(SDL_GetError()));
        }

        render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!render) {
            SDL_DestroyWindow(window);
            SDL_Quit();
            throw std::runtime_error("SDL_CreateRenderer failed: " + std::string(SDL_GetError()));
        }

        status = RUNNING;
    }

    void AddEntity(Bird* entity) {
        entities.push_back(entity);
    }

    void MainLoop(sol::state& lua) {
        sol::protected_function update = lua["update"];
        if (!update.valid()) {
            throw std::runtime_error("Lua function 'update' not found");
        }

        while (status == RUNNING) {
            // Handle input
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    status = STOPED;
                } else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_SPACE) {
                        lua["space_pressed"] = true;
                    }
                } else if (event.type == SDL_KEYUP) {
                    if (event.key.keysym.sym == SDLK_SPACE) {
                        lua["space_pressed"] = false;
                    }
                }
            }

            // Update Lua
            sol::protected_function_result update_result = update(dt);
            if (!update_result.valid()) {
                sol::error err = update_result;
                std::cerr << "Erro ao executar update: " << err.what() << std::endl;
                status = STOPED;
            }

            // Render
            SDL_SetRenderDrawColor(render, 0, 0, 0, 255); // Black background
            SDL_RenderClear(render);

            // Render entities
            for (Bird* entity : entities) {
                auto pos = entity->GetPosition();
                auto size = entity->GetSize();
                SDL_SetRenderDrawColor(render, 255, 255, 0, 255); // Yellow bird
                SDL_Rect rect = {
                    static_cast<int>(pos.x),
                    static_cast<int>(pos.y),
                    static_cast<int>(size.x),
                    static_cast<int>(size.y)
                };
                SDL_RenderFillRect(render, &rect);
            }

            SDL_RenderPresent(render);
            SDL_Delay(1000 / 60); // ~60 FPS
        }
    }
};

#endif