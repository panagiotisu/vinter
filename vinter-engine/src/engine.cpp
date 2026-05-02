#include "vinter/engine.hpp"

#include <SDL3/SDL.h> // Temporary for early debugging.

namespace vn {
    Engine::Engine(const ProjectSettings& projectSettings) {
        if (!SDL_Init(
                SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD
                | SDL_INIT_JOYSTICK
            )) {
            throw std::runtime_error(SDL_GetError());
        }

        // Forgo member initialization list to initialize SDL before other systems.
        // TODO: Bring back member initialization for Engine constructor or find better alternative.
        m_window = std::make_unique<Window>(projectSettings.window);
        m_renderer = Renderer::Create(projectSettings.renderer, *m_window);
        m_time = std::make_unique<Time>();
        m_devices = std::make_unique<DeviceManager>();
        m_input = std::make_unique<InputMap>(*m_devices);
    }

    Engine::~Engine() {
        SDL_Quit();
    }

    void Engine::Run() {
        m_running = true;

        Load();

        while (m_running) {
            SDL_Event sdlEvent;
            while (SDL_PollEvent(&sdlEvent)) {
                if (sdlEvent.type == SDL_EVENT_QUIT) {
                    m_running = false;
                }
                m_window->HandleEvents(sdlEvent);
                m_devices->HandleEvents(sdlEvent);
            }
            PollEvents();

            m_time->Update();
            Update(m_time->GetDelta());
            m_devices->Update();

            m_renderer->BeginFrame();
            Render();
            m_renderer->EndFrame();
        }
    }

    void Engine::Quit() {
        m_running = false;
    }
} // namespace vn