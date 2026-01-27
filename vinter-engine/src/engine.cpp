#include "vinter/engine.hpp"

#include <SDL3/SDL.h> // Temporary for early debugging.

#include "../include/vinter/input/keyboard.hpp"

namespace vn {
    Engine::Engine(const ProjectSettings& project_settings) {
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
        keyboard = std::make_unique<Keyboard>();
        mouse = std::make_unique<Mouse>();
    }

    Engine::~Engine() { SDL_Quit(); }

    void Engine::run() {
        m_running = true;

        load();

        while (m_running) {
            SDL_Event sdl_event;
            while (SDL_PollEvent(&sdl_event)) {
                if (
                    sdl_event.type == SDL_EVENT_QUIT ||
                    (sdl_event.type == SDL_EVENT_KEY_DOWN && sdl_event.key.key == SDLK_ESCAPE)
                ) {
                    m_running = false;
                }

                if (sdl_event.type == SDL_EVENT_MOUSE_WHEEL) {
                    mouse->handle_wheel_event(sdl_event.wheel.y);
                }
            }

            poll_events();

            time->update();
            update(time->get_delta());
            keyboard->update();
            mouse->update();

            renderer->begin_frame();
            render();
            renderer->end_frame();
        }
    }
} // vn