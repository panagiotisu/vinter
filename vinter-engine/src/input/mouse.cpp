#include "vinter/input/mouse.hpp"

#include <SDL3/SDL.h>

namespace vn {
    static std::size_t to_sdl_mouse_button(Mouse::Button button) {
        return static_cast<std::size_t>(button);
    }

    [[nodiscard]] bool Mouse::is_button_pressed(const Button button) const {
        return m_buttons.is_pressed(to_sdl_mouse_button(button));
    }
    [[nodiscard]] bool Mouse::is_button_just_pressed(const Button button) const {
        return m_buttons.is_just_pressed(to_sdl_mouse_button(button));
    }
    [[nodiscard]] bool Mouse::is_button_just_released(const Button button) const {
        return m_buttons.is_just_released(to_sdl_mouse_button(button));
    }

    glm::vec2 Mouse::get_position() const {
        return m_position;
    }

    glm::vec2 Mouse::get_delta() const {
        return m_position - m_position_previous;
    }

    glm::vec2 Mouse::get_scroll_delta() const {
        return m_scroll_delta;
    }

    bool Mouse::is_cursor_visible() const {
        return SDL_CursorVisible();
    }

    void Mouse::set_cursor_visible(const bool visible) const {
        if (visible) SDL_ShowCursor();
        else SDL_HideCursor();
    }

    void Mouse::handle_events(const SDL_Event& event) {
        if (event.type == SDL_EVENT_MOUSE_WHEEL) {
            m_scroll_delta.y += event.wheel.y;
            m_scroll_delta.x += event.wheel.x;
        }
    }

    void Mouse::update() {
        m_buttons.update();

        m_position_previous = m_position;

        const SDL_MouseButtonFlags sdl_buttons = SDL_GetMouseState(&m_position.x, &m_position.y);
        m_buttons.state_current[0] = (sdl_buttons & SDL_BUTTON_LMASK)  != 0;
        m_buttons.state_current[1] = (sdl_buttons & SDL_BUTTON_RMASK)  != 0;
        m_buttons.state_current[2] = (sdl_buttons & SDL_BUTTON_MMASK)  != 0;
        m_buttons.state_current[3] = (sdl_buttons & SDL_BUTTON_X1MASK) != 0;
        m_buttons.state_current[4] = (sdl_buttons & SDL_BUTTON_X2MASK) != 0;

        m_scroll_delta = m_scroll; // Delta since last frame.
        m_scroll = { 0.f, 0.f };
    }
} // vn