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
        SDL_Quit();
    }

    void Engine::run() {
        Logger::info("Executing Vinter Engine runtime");
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
            poll_events();

            m_time->update();
            update(m_time->get_delta());
            m_devices->update();

            render();
        }
    }

    void Engine::quit() {
        Logger::info("Game loop terminated successfully");
        Logger::info("Shutting down");
        m_running = false;
    }

    auto Engine::get_window() noexcept -> const Window& {
        return *m_window;
    }

    auto Engine::get_renderer() noexcept -> const Renderer& {
        return *m_renderer;
    }

    auto Engine::get_time() noexcept -> const Time& {
        return *m_time;
    }

    auto Engine::get_devices() noexcept -> const DeviceManager& {
        return *m_devices;
    }

    auto Engine::get_input() noexcept -> const InputMap& {
        return *m_input;
    }
} // namespace vn