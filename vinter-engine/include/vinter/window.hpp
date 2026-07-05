#pragma once

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
        void handle_events(const SDL_Event& event);

        static auto to_sdl_window_flags(const WindowSettings::Flags& flags) -> std::uint64_t;

    private:
        WindowSettings::Size m_size {};

        SDL_Window* m_backend {};
    };
} // namespace vn