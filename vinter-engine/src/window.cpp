#include "vinter/window.hpp"

#include <SDL3/SDL.h>

#include "vinter/logger.hpp"
#include "vinter/panic.hpp"
#include "vinter/settings/window_settings.hpp"

namespace vn {
    struct Window::Impl {
        SDL_Window* backend {};

        Impl(const Impl&) = default;
        Impl(Impl&&) = delete;
        auto operator=(const Impl&) -> Impl& = default;
        auto operator=(Impl&&) -> Impl& = delete;

        explicit Impl(const WindowSettings& window_settings)
            : backend(SDL_CreateWindow(
                  window_settings.title.c_str(),
                  window_settings.initial_size.width,
                  window_settings.initial_size.height,
                  to_sdl_window_flags(window_settings.flags)
              )) {
            if (backend == nullptr) {
                VN_FATAL("Failed to create Window: ", SDL_GetError());
            }
            Logger::info("Window context created successfully");
        }

        ~Impl() {
            Logger::info("Destroying Window context...");
            if (backend != nullptr) {
                SDL_DestroyWindow(backend);
            }
            Logger::info("Window context destroyed successfully");
        }

        static auto to_sdl_window_flags(const WindowSettings::Flags& flags) -> SDL_WindowFlags {
            SDL_WindowFlags sdl_window_flags = 0;

            if (flags.fullscreen) {
                sdl_window_flags |= SDL_WINDOW_FULLSCREEN;
            }
            if (flags.resizeable) {
                sdl_window_flags |= SDL_WINDOW_RESIZABLE;
            }
            if (flags.maximized) {
                sdl_window_flags |= SDL_WINDOW_MAXIMIZED;
            }
            if (flags.minimized) {
                sdl_window_flags |= SDL_WINDOW_MINIMIZED;
            }
            if (flags.hidden) {
                sdl_window_flags |= SDL_WINDOW_HIDDEN;
            }
            if (flags.borderless) {
                sdl_window_flags |= SDL_WINDOW_BORDERLESS;
            }
            if (flags.always_on_top) {
                sdl_window_flags |= SDL_WINDOW_ALWAYS_ON_TOP;
            }
            if (flags.mouse_captured) {
                sdl_window_flags |= SDL_WINDOW_MOUSE_CAPTURE;
            }
            if (flags.mouse_grabbed) {
                sdl_window_flags |= SDL_WINDOW_MOUSE_GRABBED;
            }
            if (flags.mouse_focus) {
                sdl_window_flags |= SDL_WINDOW_MOUSE_FOCUS;
            }
            if (flags.mouse_relative_mode) {
                sdl_window_flags |= SDL_WINDOW_MOUSE_RELATIVE_MODE;
            }
            if (flags.keyboard_grabbed) {
                sdl_window_flags |= SDL_WINDOW_KEYBOARD_GRABBED;
            }

            return sdl_window_flags;
        }
    };

    Window::Window(const WindowSettings& window_settings)
        : m_impl(std::make_unique<Impl>(window_settings)) {
    }

    Window::~Window() = default;

    auto Window::get_native_handle() const -> SDL_Window* {
        return m_impl->backend;
    }

    void Window::handle_events(const SDL_Event& event) {
        if (event.type == SDL_EVENT_WINDOW_RESIZED) {
            m_width = event.window.data1;
            m_height = event.window.data2;
        }
    }
} // namespace vn