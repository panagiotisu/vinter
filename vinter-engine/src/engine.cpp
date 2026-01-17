#include "vinter/engine.hpp"

#include <SDL3/SDL.h> // Temporary for early debugging.

#include "vinter/window.hpp"

namespace vn {
    Engine::Engine() {
        if (!SDL_Init(
            SDL_INIT_VIDEO |
            SDL_INIT_AUDIO |
            SDL_INIT_EVENTS |
            SDL_INIT_GAMEPAD |
            SDL_INIT_JOYSTICK
        )) {
            throw std::runtime_error(SDL_GetError());
        }
    }

    Engine::~Engine() {
        SDL_Quit();
    }

    void Engine::run() {
        m_running = true;

        load();
        while (m_running) {
            poll_events();
            update();
            render();
        }
    }

    void Engine::poll_events() {
        SDL_Event sdl_event;
        while (SDL_PollEvent(&sdl_event)) {
            if (
                sdl_event.type == SDL_EVENT_QUIT ||
                (sdl_event.type == SDL_EVENT_KEY_DOWN && sdl_event.key.key == SDLK_ESCAPE)
            ) {
                m_running = false;
            }
        }
    }
} // vn