#include "vinter/engine.hpp"

#include <SDL3/SDL.h> // Temporary for early debugging.

#include "vinter/panic.hpp"

namespace vn {
    Engine::Engine(const ProjectSettings& project_settings) {
        Logger::info("Started {}", project_settings.window.title);

        Logger::info("Initializing SDL...");
        if (!SDL_Init(
                SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD
                | SDL_INIT_JOYSTICK
            )) {
            VN_FATAL("Failed to initialize SDL: ", SDL_GetError());
        }
        Logger::info("SDL initialized successfully");

        // Forgo member initialization list to initialize SDL before other systems.
        // TODO: Bring back member initialization for Engine constructor or find better alternative.
        m_window = std::make_unique<Window>(project_settings.window);
        m_renderer = std::make_unique<Renderer>(project_settings.renderer, *m_window);
        m_time = std::make_unique<Time>();
        m_devices = std::make_unique<DeviceManager>();
        m_input = std::make_unique<InputMap>(*m_devices);
    }

    Engine::~Engine() {
        Logger::info("Destroying Vinter runtime...");
        SDL_Quit();
        Logger::info("Shutting down");
    }

    void Engine::run() {
        Logger::info("Executing Vinter runtime");
        m_running = true;

        Logger::info("Loading assets...");
        load();
        Logger::info("Assets loaded successfully");

        Logger::info("Starting game loop");
        while (m_running) {
            SDL_Event sdl_event;
            while (SDL_PollEvent(&sdl_event)) {
                if (sdl_event.type == SDL_EVENT_QUIT) {
                    m_running = false;
                }
                m_window->handle_events(sdl_event);
                m_devices->handle_events(sdl_event);
            }

            m_time->update();
            update();
            m_devices->update();

            m_renderer->begin_frame();
            render();
            m_renderer->end_frame();
        }
    }

    void Engine::quit() {
        Logger::info("Game loop terminated successfully");
        m_running = false;
    }

    auto Engine::get_window() noexcept -> Window& {
        return *m_window;
    }

    auto Engine::get_renderer() noexcept -> Renderer& {
        return *m_renderer;
    }

    auto Engine::get_time() noexcept -> Time& {
        return *m_time;
    }

    auto Engine::get_devices() noexcept -> DeviceManager& {
        return *m_devices;
    }

    auto Engine::get_input() noexcept -> InputMap& {
        return *m_input;
    }
} // namespace vn