#pragma once

#include "vinter/settings/window_settings.hpp"

struct SDL_Window;
union SDL_Event;

namespace vn {
    struct WindowSettings;

    class Window {
        friend class App;

    public:
        explicit Window(const WindowSettings& window_settings);
        ~Window();

        [[nodiscard]]
        std::uint32_t get_width() const noexcept;
        [[nodiscard]]
        std::uint32_t get_height() const noexcept;
        [[nodiscard]]
        WindowSettings::Size get_size() const noexcept;

        [[nodiscard]]
        SDL_Window* get_native_handle() const;

    private:
        static std::uint64_t to_native_window_flags(const WindowSettings::Flags& flags);

    private:
        SDL_Window* m_handle {};
    };
} // namespace vn