#include "vinter/app.hpp"

#include <SDL3/SDL.h>

#include "vinter/logger.hpp"

namespace vn {
    App::App(const ProjectSettings& project_settings) {
        VN_INFO("Started {}", project_settings.window.title);

        // Forgo member initialization list to initialize SDL before other systems.
        // TODO: Bring back member initialization for Engine constructor or find better alternative.
        m_window = std::make_unique<Window>(project_settings.window);
        m_renderer = std::make_unique<Renderer>(
            project_settings.renderer, m_window->get_native_handle()
        );
        m_time = std::make_unique<Time>();
        m_devices = std::make_unique<DeviceManager>();
        m_input = std::make_unique<InputMap>(*m_devices);
    }

    App::~App() {
        VN_INFO("Destroying Vinter runtime...");

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

    auto App::get_window() noexcept -> Window& {
        return *m_window;
    }

    auto App::get_renderer() noexcept -> Renderer& {
        return *m_renderer;
    }

    auto App::get_time() noexcept -> Time& {
        return *m_time;
    }

    auto App::get_devices() noexcept -> DeviceManager& {
        return *m_devices;
    }

    auto App::get_input() noexcept -> InputMap& {
        return *m_input;
    }
} // namespace vn