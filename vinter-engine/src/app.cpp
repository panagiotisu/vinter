#include "vinter/app.hpp"

#include <memory>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "vinter/logger.hpp"

namespace vn {
    App::SDLContext::SDLContext(int init_flags) {
        VN_INFO("Started Vinter Engine.");
        SDL_Init(init_flags);
        TTF_Init();
        VN_INFO("Running on platform: {}", SDL_GetPlatform());
    }

    App::SDLContext::~SDLContext() {
        TTF_Quit();
        SDL_Quit();
        VN_INFO("Shutting down.");
    }

    App::App(const ProjectSettings& project_settings)
        : m_ctx(
              std::make_unique<SDLContext>(
                  SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC
                  | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS | SDL_INIT_SENSOR | SDL_INIT_CAMERA
              )
          )
        , m_textures(std::make_unique<TextureManager>(project_settings.textures))
        , m_fonts(std::make_unique<FontManager>())
        , m_window(std::make_unique<Window>(project_settings.window))
        , m_renderer(
              std::make_unique<Renderer>(
                  project_settings.renderer,
                  *m_window,
                  *m_textures,
                  *m_fonts
              )
          )
        , m_time(std::make_unique<Time>())
        , m_devices(std::make_unique<Devices>())
        , m_input(std::make_unique<InputMap>(*m_devices))
        , m_database(std::make_unique<ecs::Database>())
        , m_systems(std::make_unique<ecs::SystemQueue>()) {
        VN_INFO("Vinter Engine subsystems initialized successfully.");
        VN_INFO("Started {}", project_settings.window.title);
        m_textures->attach_renderer(*m_renderer);
    }

    App::~App() {
        VN_INFO("Destroying Vinter runtime...");
    }

    void App::run() {
        VN_INFO("Executing Vinter runtime...");
        m_running = true;

        VN_INFO("Loading assets...");
        load();
        VN_INFO("Assets loaded successfully.");

        m_systems->pre_update();

        VN_INFO("Starting game loop...");
        while (m_running) {
            SDL_Event native_event;
            while (SDL_PollEvent(&native_event)) {
                if (native_event.type == SDL_EVENT_QUIT) {
                    m_running = false;
                }
                handle_debug_gui_events(native_event);
                m_devices->handle_events(native_event);
            }

            m_time->update();
            const auto delta = m_time->get_delta_time();
            update(delta);
            m_systems->update(delta);
            m_devices->update();

            m_renderer->begin_frame();
            render();
            m_systems->render();
            m_renderer->end_frame();
        }

        m_systems->post_update();
        unload();
    }

    void App::quit() {
        VN_INFO("Game loop terminated.");
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

    Devices& App::get_devices() noexcept {
        return *m_devices;
    }

    InputMap& App::get_input() noexcept {
        return *m_input;
    }

    ecs::Database& App::get_ecs() noexcept {
        return *m_database;
    }

    ecs::SystemQueue& App::get_systems() noexcept {
        return *m_systems;
    }

    TextureManager& App::get_textures() noexcept {
        return *m_textures;
    }

    FontManager& App::get_fonts() noexcept {
        return *m_fonts;
    }
} // namespace vn