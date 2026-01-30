#pragma once

#include <glm/glm.hpp>

#include <memory>

namespace vn {
    enum class MouseButton {
        Left, Right, Middle, X1, X2,
    };

    class Mouse {
        friend class Engine;

    public:
        Mouse();
        ~Mouse();

        [[nodiscard]] bool is_button_pressed(MouseButton button) const;
        [[nodiscard]] bool is_button_just_pressed(MouseButton button) const;
        [[nodiscard]] bool is_button_just_released(MouseButton button) const;

        [[nodiscard]] glm::vec2 get_position() const;
        [[nodiscard]] glm::vec2 get_delta() const;
        [[nodiscard]] int get_scroll_delta() const;

        void set_cursor_visible(bool visible) const;

    private:
        void handle_wheel_event(float delta);
        void update();

        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // vn