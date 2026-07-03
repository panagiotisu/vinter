#pragma once

#include <glm/glm.hpp>

#include "vinter/input/button_states.hpp"

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
        auto is_button_pressed(Button button) const -> bool;

        [[nodiscard]]
        auto is_button_just_pressed(Button button) const -> bool;

        [[nodiscard]]
        auto is_button_just_released(Button button) const -> bool;

        [[nodiscard]]
        auto is_wheel_triggered(Wheel wheel) const -> bool;

        [[nodiscard]]
        auto get_position() const -> glm::vec2;

        [[nodiscard]]
        auto get_delta() const -> glm::vec2;

        [[nodiscard]]
        auto get_scroll() const -> glm::vec2;

        [[nodiscard]]
        auto get_scroll_vertical() const -> float;

        [[nodiscard]]
        auto get_scroll_horizontal() const -> float;

        [[nodiscard]]
        static auto is_cursor_visible() -> bool;

        static void set_cursor_visible(bool visible);

    private:
        void handle_events(const SDL_Event& event);
        void update();

        ButtonStates<5> m_buttons {};
        glm::Vec2 m_position {};
        glm::Vec2 m_position_previous {};
        glm::Vec2 m_scroll {};
    };
} // namespace vn
