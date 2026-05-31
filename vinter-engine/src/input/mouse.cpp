#include "vinter/input/mouse.hpp"

#include <SDL3/SDL.h>

namespace vn
{
    static auto to_sdl_mouse_button(Mouse::Button button) -> std::size_t
    {
        return static_cast<std::size_t>(button);
    }

    [[nodiscard]]
    auto Mouse::is_button_pressed(const Button button) const -> bool
    {
        return m_buttons.is_pressed(to_sdl_mouse_button(button));
    }

    [[nodiscard]]
    auto Mouse::is_button_just_pressed(const Button button) const -> bool
    {
        return m_buttons.is_just_pressed(to_sdl_mouse_button(button));
    }

    [[nodiscard]]
    auto Mouse::is_button_just_released(const Button button) const -> bool
    {
        return m_buttons.is_just_released(to_sdl_mouse_button(button));
    }

    auto Mouse::is_wheel_triggered(const Wheel wheel) const -> bool
    {
        switch (wheel)
        {
            case Wheel::Up: return m_scroll.y > 0;
            case Wheel::Down: return m_scroll.y < 0;
            case Wheel::Right: return m_scroll.x > 0;
            case Wheel::Left: return m_scroll.x < 0;
        }
        return false;
    }

    auto Mouse::get_position() const -> glm::vec2
    {
        return m_position;
    }

    auto Mouse::get_delta() const -> glm::vec2
    {
        return m_position - m_position_previous;
    }

    auto Mouse::get_scroll() const -> glm::vec2
    {
        return m_scroll;
    }

    auto Mouse::get_scroll_vertical() const -> float
    {
        return m_scroll.y;
    }

    auto Mouse::get_scroll_horizontal() const -> float
    {
        return m_scroll.x;
    }

    auto Mouse::is_cursor_visible() -> bool
    {
        return SDL_CursorVisible();
    }

    void Mouse::set_cursor_visible(const bool visible)
    {
        if (visible)
        {
            SDL_ShowCursor();
        }
        else
        {
            SDL_HideCursor();
        }
    }

    void Mouse::handle_events(const SDL_Event& event)
    {
        if (event.type == SDL_EVENT_MOUSE_WHEEL)
        {
            m_scroll += glm::vec2(event.wheel.x, event.wheel.y);
        }
    }

    void Mouse::update()
    {
        m_buttons.refresh();
        m_position_previous = m_position;
        m_scroll = {0.f, 0.f};

        const SDL_MouseButtonFlags sdl_buttons = SDL_GetMouseState(&m_position.x, &m_position.y);
        m_buttons.current[0] = (sdl_buttons & SDL_BUTTON_LMASK) != 0;
        m_buttons.current[1] = (sdl_buttons & SDL_BUTTON_RMASK) != 0;
        m_buttons.current[2] = (sdl_buttons & SDL_BUTTON_MMASK) != 0;
        m_buttons.current[3] = (sdl_buttons & SDL_BUTTON_X1MASK) != 0;
        m_buttons.current[4] = (sdl_buttons & SDL_BUTTON_X2MASK) != 0;
    }
} // namespace vn