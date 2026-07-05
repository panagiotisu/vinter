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
        auto is_button_pressed(Button button) const noexcept -> bool;

        [[nodiscard]]
        auto is_button_just_pressed(Button button) const noexcept -> bool;

        [[nodiscard]]
        auto is_button_just_released(Button button) const noexcept -> bool;

        [[nodiscard]]
        auto is_wheel_triggered(Wheel wheel) const noexcept -> bool;

        [[nodiscard]]
        auto get_position() const noexcept -> glm::vec2;

        [[nodiscard]]
        auto get_delta() const noexcept -> glm::vec2;

        [[nodiscard]]
        auto get_scroll() const noexcept -> glm::vec2;

        [[nodiscard]]
        auto get_scroll_vertical() const noexcept -> float;

        [[nodiscard]]
        auto get_scroll_horizontal() const noexcept -> float;

        [[nodiscard]]
        auto is_cursor_visible() const noexcept -> bool;
        void set_cursor_visible(bool visible);

    private:
        void handle_events(const SDL_Event& event);
        void update();

        [[nodiscard]]
        static auto to_sdl_mouse_button(Mouse::Button button) noexcept -> std::size_t;

    private:
        static constexpr std::size_t ButtonCount { 5 };
        ButtonStates m_buttons { ButtonCount };
        glm::vec2 m_position {};
        glm::vec2 m_position_previous {};
        glm::vec2 m_scroll {};
    };
} // namespace vn
