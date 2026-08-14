#pragma once

#include <glm/glm.hpp>

#include "vinter/input/input_states.hpp"

union SDL_Event;

namespace vn {
    class Mouse {
        friend class DeviceManager;

    public:
        enum class Button : std::uint8_t {
            Left,
            Right,
            Middle,
            X1,
            X2,
        };

        enum class Wheel : std::uint8_t {
            Up,
            Down,
            Left,
            Right,
        };

        [[nodiscard]]
        bool is_button_pressed(Button button) const noexcept;

        [[nodiscard]]
        bool is_button_just_pressed(Button button) const noexcept;

        [[nodiscard]]
        bool is_button_just_released(Button button) const noexcept;

        [[nodiscard]]
        bool is_wheel_triggered(Wheel wheel) const noexcept;

        [[nodiscard]]
        glm::vec2 get_position() const noexcept;

        [[nodiscard]]
        glm::vec2 get_delta() const noexcept;

        [[nodiscard]]
        glm::vec2 get_scroll() const noexcept;

        [[nodiscard]]
        float get_scroll_vertical() const noexcept;

        [[nodiscard]]
        float get_scroll_horizontal() const noexcept;

        [[nodiscard]]
        bool is_cursor_visible() const noexcept;
        void set_cursor_visible(bool visible);

    private:
        void handle_events(const SDL_Event& event);
        void update();

        [[nodiscard]]
        static std::size_t to_sdl_mouse_button(Mouse::Button button) noexcept;

    private:
        static constexpr std::size_t ButtonCount { 5 };
        InputStates<bool> m_buttons { ButtonCount };
        glm::vec2 m_position {};
        glm::vec2 m_position_previous {};
        glm::vec2 m_scroll {};
    };
} // namespace vn