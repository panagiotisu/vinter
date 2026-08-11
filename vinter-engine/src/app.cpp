#include "vinter/app.hpp"

#include <SDL3/SDL.h>

#include "vinter/logger.hpp"

namespace vn {
    App::App(const ProjectSettings& project_settings) {
        VN_INFO("Started {}", project_settings.window.title);

        // Forgo member initialization list to initialize SDL before other systems.
        // TODO: Bring back member initialization for Engine constructor or find better alternative.
        m_window = std::make_unique<Window>(project_settings.window);
        m_renderer = std::make_unique<Renderer>(project_settings.renderer, *m_window);
        m_time = std::make_unique<Time>();
        m_devices = std::make_unique<DeviceManager>();
        m_input = std::make_unique<InputMap>(*m_devices);
        m_ecs = std::make_unique<ECS>();
    }

    App::~App() {
        VN_INFO("Destroying Vinter runtime...");

        m_ecs->reset();
        m_input.reset();
        m_devices.reset();
        m_time.reset();
        m_renderer.reset();
        m_window.reset();

        SDL_Quit();
        VN_INFO("Shutting down");
    }

    void App::run() {
        VN_INFO("Executing Vinter runtime");
        m_running = true;

        VN_INFO("Loading assets...");
        load();
        VN_INFO("Assets loaded successfully");

        VN_INFO("Starting game loop");
        while (m_running) {
            SDL_Event sdl_event;
            while (SDL_PollEvent(&sdl_event)) {
                if (sdl_event.type == SDL_EVENT_QUIT) {
                    m_running = false;
                }
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

    void App::quit() {
        VN_INFO("Game loop terminated successfully");
        m_running = false;
    }

    Window& App::get_window() noexcept {
        return *m_window;
    }

    Renderer& App::get_renderer() noexcept {
        return *m_renderer;
    }

    Time& App::get_time() noexcept {
        return *m_time;
    }

    DeviceManager& App::get_devices() noexcept {
        return *m_devices;
    }

    InputMap& App::get_input() noexcept {
        return *m_input;
    }

    ECS& App::get_ecs() noexcept {
        return *m_ecs;
    }
} // namespace vn