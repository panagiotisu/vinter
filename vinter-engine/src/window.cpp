#include "vinter/window.hpp"

#include <SDL3/SDL.h>

#include "vinter/settings/window_settings.hpp"

namespace vn {
    struct Window::Impl {
        SDL_Window* sdl_window_backend;

        static SDL_WindowFlags to_sdl_window_flags(const WindowSettings::Flags& flags) {
            SDL_WindowFlags sdl_window_flags = 0;

            if (flags.fullscreen) sdl_window_flags          |= SDL_WINDOW_FULLSCREEN;
            if (flags.resizeable) sdl_window_flags          |= SDL_WINDOW_RESIZABLE;
            if (flags.maximized) sdl_window_flags           |= SDL_WINDOW_MAXIMIZED;
            if (flags.minimized) sdl_window_flags           |= SDL_WINDOW_MINIMIZED;
            if (flags.hidden) sdl_window_flags              |= SDL_WINDOW_HIDDEN;
            if (flags.borderless) sdl_window_flags          |= SDL_WINDOW_BORDERLESS;
            if (flags.always_on_top) sdl_window_flags       |= SDL_WINDOW_ALWAYS_ON_TOP;
            if (flags.mouse_captured) sdl_window_flags      |= SDL_WINDOW_MOUSE_CAPTURE;
            if (flags.mouse_grabbed) sdl_window_flags       |= SDL_WINDOW_MOUSE_GRABBED;
            if (flags.mouse_focus) sdl_window_flags         |= SDL_WINDOW_MOUSE_FOCUS;
            if (flags.mouse_relative_mode) sdl_window_flags |= SDL_WINDOW_MOUSE_RELATIVE_MODE;
            if (flags.keyboard_grabbed) sdl_window_flags    |= SDL_WINDOW_KEYBOARD_GRABBED;

            return sdl_window_flags;
        }
    };

    Window::Window(const WindowSettings &window_settings) {
    }

    Window::~Window() {
        SDL_DestroyWindow(m_impl->sdl_window_backend);
    }
} // vn