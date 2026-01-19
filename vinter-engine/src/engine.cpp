#include "vinter/engine.hpp"

#include <SDL3/SDL.h> // Temporary for early debugging.

#include "vinter/settings/project_settings.hpp"
#include "vinter/window.hpp"
#include "vinter/renderer.hpp"
#include "vinter/time.hpp"

namespace vn {
    Engine::Engine(const ProjectSettings &project_settings) {
        if (!SDL_Init(
            SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK
        )) {
            throw std::runtime_error(SDL_GetError());
        }

        // Forgo member initialization list to initialize SDL before other systems.
        // TODO: Bring back member initialization for Engine constructor or find better alternative.
        window = std::make_unique<Window>(project_settings.window);
        renderer = Renderer::create(project_settings.renderer, *window);
        time = std::make_unique<Time>();
    }

    Engine::~Engine() {
        SDL_Quit();
    }

    void Engine::run() {
        m_running = true;

        load();
        while (m_running) {
            poll_events();

            time->update();
            update(time->get_delta());

            renderer->begin_frame();
            render();
            renderer->end_frame();
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