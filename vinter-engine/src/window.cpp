#include "vinter/window.hpp"

#include <SDL3/SDL.h>

#include "vinter/logger.hpp"
#include "vinter/settings/window_settings.hpp"

namespace vn {
    Window::Window(const WindowSettings& window_settings)
        : m_handle(SDL_CreateWindow(
              window_settings.title.c_str(),
              static_cast<int>(window_settings.initial_size.width),
              static_cast<int>(window_settings.initial_size.height),
              to_sdl_window_flags(window_settings.flags)
          )) {
        if (m_handle == nullptr) {
            VN_FATAL("Failed to create Window: ", SDL_GetError());
        }
        VN_INFO("Window context created successfully");
    }

    Window::~Window() {
        VN_INFO("Destroying Window context...");
        if (m_handle != nullptr) {
            SDL_DestroyWindow(m_handle);
        }
        VN_INFO("Window context destroyed successfully");
    };

    std::uint32_t Window::get_width() const noexcept {
        int w {};
        int h {};
        SDL_GetWindowSize(m_handle, &w, &h);
        return static_cast<std::uint32_t>(w);
    }

    std::uint32_t Window::get_height() const noexcept {
        int w {};
        int h {};
        SDL_GetWindowSize(m_handle, &w, &h);
        return static_cast<std::uint32_t>(h);
    }

    WindowSettings::Size Window::get_size() const noexcept {
        int w {};
        int h {};
        SDL_GetWindowSize(m_handle, &w, &h);
        return WindowSettings::Size {
            static_cast<std::uint32_t>(w),
            static_cast<std::uint32_t>(h),
        };
    }

    SDL_Window* Window::get_native_handle() const {
        return m_handle;
    }

    SDL_WindowFlags Window::to_sdl_window_flags(const WindowSettings::Flags& flags) {
        SDL_WindowFlags sdl_window_flags {};

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
} // namespace vn