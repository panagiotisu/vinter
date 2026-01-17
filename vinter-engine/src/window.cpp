#include "vinter/window.hpp"

#include <SDL3/SDL.h>

namespace vn {
    struct Window::Impl {
        SDL_Window* sdl_window_backend;
    };

    Window::~Window() {
        SDL_DestroyWindow(m_impl->sdl_window_backend);
        SDL_Quit();
    }
} // vn