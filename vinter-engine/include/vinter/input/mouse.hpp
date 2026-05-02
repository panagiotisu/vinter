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
        auto IsButtonPressed(Button button) const -> bool;

        [[nodiscard]]
        auto IsButtonJustPressed(Button button) const -> bool;

        [[nodiscard]]
        auto IsButtonJustReleased(Button button) const -> bool;

        [[nodiscard]]
        auto IsWheelTriggered(Wheel wheel) const -> bool;

        [[nodiscard]]
        auto GetPosition() const -> glm::vec2;

        [[nodiscard]]
        auto GetDelta() const -> glm::vec2;

        [[nodiscard]]
        auto GetScroll() const -> glm::vec2;

        [[nodiscard]]
        auto GetScrollVertical() const -> float;

        [[nodiscard]]
        auto GetScrollHorizontal() const -> float;

        [[nodiscard]]
        auto IsCursorVisible() const -> bool;

        void SetCursorVisible(bool visible) const;

    private:
        void HandleEvents(const SDL_Event& event);
        void Update();

        ButtonStates<5> m_buttons {};
        glm::vec2 m_position {};
        glm::vec2 m_positionPrevious {};
        glm::vec2 m_scroll {};
    };
} // namespace vn
