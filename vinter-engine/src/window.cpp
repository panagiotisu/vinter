#include "vinter/window.hpp"

#include <SDL3/SDL.h>

#include "vinter/settings/window_settings.hpp"

namespace vn {
    struct Window::Impl {
        SDL_Window* sdlWindowBackend {nullptr};

        Impl(const Impl&) = default;
        Impl(Impl&&) = delete;
        auto operator=(const Impl&) -> Impl& = default;
        auto operator=(Impl&&) -> Impl& = delete;

        explicit Impl(const WindowSettings& windowSettings)
            : sdlWindowBackend(SDL_CreateWindow(
                  windowSettings.title.c_str(),
                  windowSettings.initial_size.width,
                  windowSettings.initial_size.height,
                  ToSdlWindowFlags(windowSettings.flags)
              )) {
            if (sdlWindowBackend == nullptr) {
                throw std::runtime_error(SDL_GetError());
            }
        }

        ~Impl() {
            if (sdlWindowBackend != nullptr) {
                SDL_DestroyWindow(sdlWindowBackend);
            }
        }

        static auto ToSdlWindowFlags(const WindowSettings::Flags& flags) -> SDL_WindowFlags {
            SDL_WindowFlags sdlWindowFlags = 0;

            if (flags.fullscreen) {
                sdlWindowFlags |= SDL_WINDOW_FULLSCREEN;
            }
            if (flags.resizeable) {
                sdlWindowFlags |= SDL_WINDOW_RESIZABLE;
            }
            if (flags.maximized) {
                sdlWindowFlags |= SDL_WINDOW_MAXIMIZED;
            }
            if (flags.minimized) {
                sdlWindowFlags |= SDL_WINDOW_MINIMIZED;
            }
            if (flags.hidden) {
                sdlWindowFlags |= SDL_WINDOW_HIDDEN;
            }
            if (flags.borderless) {
                sdlWindowFlags |= SDL_WINDOW_BORDERLESS;
            }
            if (flags.always_on_top) {
                sdlWindowFlags |= SDL_WINDOW_ALWAYS_ON_TOP;
            }
            if (flags.mouse_captured) {
                sdlWindowFlags |= SDL_WINDOW_MOUSE_CAPTURE;
            }
            if (flags.mouse_grabbed) {
                sdlWindowFlags |= SDL_WINDOW_MOUSE_GRABBED;
            }
            if (flags.mouse_focus) {
                sdlWindowFlags |= SDL_WINDOW_MOUSE_FOCUS;
            }
            if (flags.mouse_relative_mode) {
                sdlWindowFlags |= SDL_WINDOW_MOUSE_RELATIVE_MODE;
            }
            if (flags.keyboard_grabbed) {
                sdlWindowFlags |= SDL_WINDOW_KEYBOARD_GRABBED;
            }

            return sdlWindowFlags;
        }
    };

    Window::Window(const WindowSettings& windowSettings)
        : m_impl(std::make_unique<Impl>(windowSettings)) {
    }

    Window::~Window() = default;

    auto Window::GetNativeHandle() const -> SDL_Window* {
        return m_impl->sdlWindowBackend;
    }

    void Window::HandleEvents(const SDL_Event& event) {
        if (event.type == SDL_EVENT_WINDOW_RESIZED) {
            m_width = event.window.data1;
            m_height = event.window.data2;
        }
    }
} // namespace vn