#pragma once

#include <memory>

struct SDL_Window;

namespace vn {
    struct WindowSettings;

    class Window {
    public:
        explicit Window(const WindowSettings& window_settings);
        ~Window();

        [[nodiscard]] SDL_Window* get_native_handle() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // vn