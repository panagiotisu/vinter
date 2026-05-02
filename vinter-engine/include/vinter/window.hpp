#pragma once

#include <memory>

struct SDL_Window;
union SDL_Event;

namespace vn {
    struct WindowSettings;

    class Window {
        friend class Engine;

    public:
        explicit Window(const WindowSettings& windowSettings);
        ~Window();

        [[nodiscard]]
        auto GetNativeHandle() const -> SDL_Window*;

    private:
        int m_width {0}, m_height {0};

        void HandleEvents(const SDL_Event& event);

        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // namespace vn