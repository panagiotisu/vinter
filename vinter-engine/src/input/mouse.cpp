#include "vinter/input/mouse.hpp"

#include <SDL3/SDL.h>

#include "buttons.hpp"

namespace vn {
    struct Mouse::Impl {
        float x { 0 }, y { 0 };
        float x_previous { 0 }, y_previous { 0 };

        float scroll { 0 };
        float scroll_delta { 0 };

        Buttons<5> buttons;

        void handle_wheel_event(const float delta) {
            scroll += delta;
        }

        void update() {
            buttons.update();

            x_previous = x;
            y_previous = y;

            const SDL_MouseButtonFlags sdl_buttons = SDL_GetMouseState(&x, &y);
            buttons.state_current[0] = (sdl_buttons & SDL_BUTTON_LMASK)  != 0;
            buttons.state_current[1] = (sdl_buttons & SDL_BUTTON_RMASK)  != 0;
            buttons.state_current[2] = (sdl_buttons & SDL_BUTTON_MMASK)  != 0;
            buttons.state_current[3] = (sdl_buttons & SDL_BUTTON_X1MASK) != 0;
            buttons.state_current[4] = (sdl_buttons & SDL_BUTTON_X2MASK) != 0;

            scroll_delta = scroll; // Delta since last frame.
            scroll = 0;
        }

        static std::size_t to_sdl_mouse_button(MouseButton button) {
            return static_cast<std::size_t>(button);
        }

        [[nodiscard]] bool is_button_pressed(const MouseButton button) const {
            return buttons.is_pressed(to_sdl_mouse_button(button));
        }

        [[nodiscard]] bool is_button_just_pressed(const MouseButton button) const {
            return buttons.is_just_pressed(to_sdl_mouse_button(button));
        }

        [[nodiscard]] bool is_button_just_released(const MouseButton button) const {
            return buttons.is_just_released(to_sdl_mouse_button(button));
        }
    };

    Mouse::Mouse()
        : m_impl(std::make_unique<Impl>()) {
    }

    Mouse::~Mouse() = default;

    glm::vec2 Mouse::get_position() const {
        return glm::vec2 { m_impl->x, m_impl->y };
    }

    glm::vec2 Mouse::get_delta() const {
        return glm::vec2 {
            m_impl->x - m_impl->x_previous,
            m_impl->y - m_impl->y_previous
        };
    }

    float Mouse::get_scroll_delta() const {
        return m_impl->scroll_delta;
    }

    bool Mouse::is_cursor_visible() const {
        return SDL_CursorVisible();
    }

    void Mouse::set_cursor_visible(const bool visible) const {
        if (visible) SDL_ShowCursor();
        else SDL_HideCursor();
    }

    bool Mouse::is_button_pressed(const MouseButton button) const {
        return m_impl->is_button_pressed(button);
    }

    bool Mouse::is_button_just_pressed(const MouseButton button) const {
        return m_impl->is_button_just_pressed(button);
    }

    bool Mouse::is_button_just_released(const MouseButton button) const {
        return m_impl->is_button_just_released(button);
    }

    void Mouse::handle_wheel_event(const float delta) {
        m_impl->handle_wheel_event(delta);
    }

    void Mouse::update() {
        m_impl->update();
    }
} // vn