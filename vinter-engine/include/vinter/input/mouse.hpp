#pragma once

#include <glm/glm.hpp>

#include "vinter/input/buttons.hpp"

union SDL_Event;

namespace vn {
    class Mouse {
        friend class DeviceManager;

    public:
        enum class Button {
            Left, Right, Middle, X1, X2,
        };

        enum class Wheel {
            Up, Down, Left, Right,
        };

        [[nodiscard]] bool is_button_pressed(Button button) const;
        [[nodiscard]] bool is_button_just_pressed(Button button) const;
        [[nodiscard]] bool is_button_just_released(Button button) const;

        [[nodiscard]] glm::vec2 get_position() const;
        [[nodiscard]] glm::vec2 get_delta() const;
        [[nodiscard]] glm::vec2 get_scroll() const;
        [[nodiscard]] float get_scroll_vertical() const;
        [[nodiscard]] float get_scroll_horizontal() const;

        [[nodiscard]] bool is_cursor_visible() const;
        void set_cursor_visible(bool visible) const;

    private:
        void handle_events(const SDL_Event& event);
        void update();

        Buttons<5> m_buttons {};
        glm::vec2 m_position {};
        glm::vec2 m_position_previous {};
        glm::vec2 m_scroll {};
    };
} // vn
