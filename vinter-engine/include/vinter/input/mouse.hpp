#pragma once

#include <glm/glm.hpp>

#include "vinter/input/buttons.hpp"

namespace vn {
    enum class MouseButton {
        Left, Right, Middle, X1, X2,
    };

    class Mouse {
        friend class Engine;

    public:
        [[nodiscard]] bool is_button_pressed(MouseButton button) const;
        [[nodiscard]] bool is_button_just_pressed(MouseButton button) const;
        [[nodiscard]] bool is_button_just_released(MouseButton button) const;

        [[nodiscard]] glm::vec2 get_position() const;
        [[nodiscard]] glm::vec2 get_delta() const;
        [[nodiscard]] float get_scroll_delta() const;

        [[nodiscard]] bool is_cursor_visible() const;
        void set_cursor_visible(bool visible) const;

    private:
        void handle_wheel_event(float delta);
        void update();

        Buttons<5> m_buttons {};
        glm::vec2 m_position;
        glm::vec2 m_position_previous;
        float m_scroll;
        float m_scroll_delta;
    };
} // vn
