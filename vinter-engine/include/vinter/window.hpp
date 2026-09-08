#pragma once

#include <glm/glm.hpp>

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
        std::size_t get_width() const noexcept;
        [[nodiscard]]
        std::size_t get_height() const noexcept;
        [[nodiscard]]
        glm::uvec2 get_size() const noexcept;

        void set_title(const std::string& title);

        [[nodiscard]]
        SDL_Window* get_native_handle() const;

    private:
        static std::uint64_t to_native_window_flags(const WindowSettings::Flags& flags);

    private:
        SDL_Window* m_handle {};
    };
} // namespace vn