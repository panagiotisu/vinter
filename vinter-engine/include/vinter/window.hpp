#pragma once

#include <memory>

#include "vinter/settings/window_settings.hpp"

struct SDL_Window;
union SDL_Event;

namespace vn {
    struct WindowSettings;

    class Window {
        friend class Engine;

    public:
        explicit Window(const WindowSettings& window_settings);
        ~Window();

        [[nodiscard]]
        auto get_width() const noexcept -> WindowSettings::Dimension;

        [[nodiscard]]
        auto get_height() const noexcept -> WindowSettings::Dimension;

        [[nodiscard]]
        auto get_size() const noexcept -> WindowSettings::Size;

        [[nodiscard]]
        auto get_native_handle() const -> SDL_Window*;

    private:
        WindowSettings::Size m_size {};

        void handle_events(const SDL_Event& event);

        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // namespace vn