#include "vinter/input/mouse.hpp"

#include <SDL3/SDL.h>

namespace vn {
    static auto ToSdlMouseButton(Mouse::Button button) -> std::size_t {
        return static_cast<std::size_t>(button);
    }

    [[nodiscard]]
    auto Mouse::IsButtonPressed(const Button button) const -> bool {
        return m_buttons.IsPressed(ToSdlMouseButton(button));
    }

    [[nodiscard]]
    auto Mouse::IsButtonJustPressed(const Button button) const -> bool {
        return m_buttons.IsJustPressed(ToSdlMouseButton(button));
    }

    [[nodiscard]]
    auto Mouse::IsButtonJustReleased(const Button button) const -> bool {
        return m_buttons.IsJustReleased(ToSdlMouseButton(button));
    }

    auto Mouse::IsWheelTriggered(const Wheel wheel) const -> bool {
        switch (wheel) {
            case Wheel::Up: return m_scroll.y > 0;
            case Wheel::Down: return m_scroll.y < 0;
            case Wheel::Right: return m_scroll.x > 0;
            case Wheel::Left: return m_scroll.x < 0;
        }
        return false;
    }

    auto Mouse::GetPosition() const -> glm::vec2 {
        return m_position;
    }

    auto Mouse::GetDelta() const -> glm::vec2 {
        return m_position - m_positionPrevious;
    }

    auto Mouse::GetScroll() const -> glm::vec2 {
        return m_scroll;
    }

    auto Mouse::GetScrollVertical() const -> float {
        return m_scroll.y;
    }

    auto Mouse::GetScrollHorizontal() const -> float {
        return m_scroll.x;
    }

    auto Mouse::IsCursorVisible() const -> bool {
        return SDL_CursorVisible();
    }

    void Mouse::SetCursorVisible(const bool visible) const {
        if (visible) {
            SDL_ShowCursor();
        } else {
            SDL_HideCursor();
        }
    }

    void Mouse::HandleEvents(const SDL_Event& event) {
        if (event.type == SDL_EVENT_MOUSE_WHEEL) {
            m_scroll += glm::vec2(event.wheel.x, event.wheel.y);
        }
    }

    void Mouse::Update() {
        m_buttons.Refresh();
        m_positionPrevious = m_position;
        m_scroll = {0.f, 0.f};

        const SDL_MouseButtonFlags sdlButtons = SDL_GetMouseState(&m_position.x, &m_position.y);
        m_buttons.current[0] = (sdlButtons & SDL_BUTTON_LMASK) != 0;
        m_buttons.current[1] = (sdlButtons & SDL_BUTTON_RMASK) != 0;
        m_buttons.current[2] = (sdlButtons & SDL_BUTTON_MMASK) != 0;
        m_buttons.current[3] = (sdlButtons & SDL_BUTTON_X1MASK) != 0;
        m_buttons.current[4] = (sdlButtons & SDL_BUTTON_X2MASK) != 0;
    }
} // namespace vn