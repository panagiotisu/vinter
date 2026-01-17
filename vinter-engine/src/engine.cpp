#include "vinter/engine.hpp"

#include <SDL3/SDL.h> // Temporary for early debugging.

namespace vn {
    Engine::Engine() {}

    Engine::~Engine() {}

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