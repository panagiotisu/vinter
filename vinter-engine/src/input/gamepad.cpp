#include "vinter/input/gamepad.hpp"

#include <algorithm>
#include <array>
#include <cmath>

#include <SDL3/SDL.h>

#include "vinter/assert.hpp"
#include "vinter/color.hpp"
#include "vinter/input/button_states.hpp"

namespace vn {
    static auto NormalizeAxis(const float axis) noexcept -> float {
        if (axis < 0.f) {
            return -axis / SDL_JOYSTICK_AXIS_MIN;
        }
        return axis / SDL_JOYSTICK_AXIS_MAX;
    }

    static void ApplyTriggerDeadzone(float& triggerValue, const float deadzone) {
        if (triggerValue < deadzone) {
            triggerValue = 0.f;
        } else {
            triggerValue = (triggerValue - deadzone) / (1.f - deadzone);
        }
    }

    static void ApplyStickDeadzone(float& stickX, float& stickY, const float deadzone) {
        const float magnitudeSquared = (stickX * stickX) + (stickY * stickY);
        if (magnitudeSquared < deadzone * deadzone) {
            stickX = 0.f;
            stickY = 0.f;
        } else {
            const float magnitude = std::sqrt(magnitudeSquared);
            const float magnitudeScaled = (magnitude - deadzone) / (1.f - deadzone);

            stickX *= magnitudeScaled / magnitude;
            stickY *= magnitudeScaled / magnitude;
        }
    }

    static auto AxisToIndex(const Gamepad::Axis axis) -> std::size_t {
        return static_cast<std::size_t>(axis);
    }

    struct Gamepad::Impl {
        SDL_Gamepad* sdlGamepad {nullptr};
        ButtonStates<SDL_GAMEPAD_BUTTON_COUNT> buttonStates {};
        std::array<float, SDL_GAMEPAD_AXIS_COUNT> sdlAxisStatesCurrent {}, sdlAxisStatesPrevious {};
        std::array<float, static_cast<std::size_t>(Axis::Count)> axisStatesCurrent {},
            axisStatesPrevious {};

        Impl(const Impl&) = default;
        Impl(Impl&&) = delete;
        auto operator=(const Impl&) -> Impl& = default;
        auto operator=(Impl&&) -> Impl& = delete;

        explicit Impl(const unsigned int joystickId) : sdlGamepad(SDL_OpenGamepad(joystickId)) {
            VN_ASSERT(sdlGamepad, "Failed to open SDL gamepad.");
        }

        ~Impl() {
            if (sdlGamepad != nullptr) {
                SDL_CloseGamepad(sdlGamepad);
            }
        }

        [[nodiscard]]
        static auto ToSdlGamepadButton(const Button button) noexcept -> SDL_GamepadButton {
            switch (button) {
                default: return SDL_GAMEPAD_BUTTON_INVALID;

                case Button::South: return SDL_GAMEPAD_BUTTON_SOUTH;
                case Button::East: return SDL_GAMEPAD_BUTTON_EAST;
                case Button::West: return SDL_GAMEPAD_BUTTON_WEST;
                case Button::North: return SDL_GAMEPAD_BUTTON_NORTH;

                case Button::Back: return SDL_GAMEPAD_BUTTON_BACK;
                case Button::Guide: return SDL_GAMEPAD_BUTTON_GUIDE;
                case Button::Start: return SDL_GAMEPAD_BUTTON_START;

                case Button::LeftStick: return SDL_GAMEPAD_BUTTON_LEFT_STICK;
                case Button::RightStick: return SDL_GAMEPAD_BUTTON_RIGHT_STICK;

                case Button::LeftShoulder: return SDL_GAMEPAD_BUTTON_LEFT_SHOULDER;
                case Button::RightShoulder: return SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER;

                case Button::DpadUp: return SDL_GAMEPAD_BUTTON_DPAD_UP;
                case Button::DpadDown: return SDL_GAMEPAD_BUTTON_DPAD_DOWN;
                case Button::DpadLeft: return SDL_GAMEPAD_BUTTON_DPAD_LEFT;
                case Button::DpadRight: return SDL_GAMEPAD_BUTTON_DPAD_RIGHT;

                case Button::RightPaddle1: return SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1;
                case Button::LeftPaddle1: return SDL_GAMEPAD_BUTTON_LEFT_PADDLE1;
                case Button::RightPaddle2: return SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2;
                case Button::LeftPaddle2: return SDL_GAMEPAD_BUTTON_LEFT_PADDLE2;

                case Button::Touchpad: return SDL_GAMEPAD_BUTTON_TOUCHPAD;

                case Button::Misc1: return SDL_GAMEPAD_BUTTON_MISC1;
                case Button::Misc2: return SDL_GAMEPAD_BUTTON_MISC2;
                case Button::Misc3: return SDL_GAMEPAD_BUTTON_MISC3;
                case Button::Misc4: return SDL_GAMEPAD_BUTTON_MISC4;
                case Button::Misc5: return SDL_GAMEPAD_BUTTON_MISC5;
                case Button::Misc6: return SDL_GAMEPAD_BUTTON_MISC6;
            }
        }

        static void RemapSdlAxes(
            std::array<float, static_cast<std::size_t>(Axis::Count)>& axes,
            const std::array<float, SDL_GAMEPAD_AXIS_COUNT>& sdlAxes
        ) {
            // Split and remap stick axes so that they are always between [0, 1] instead of [-1, 1].
            axes[AxisToIndex(Axis::LeftStickLeft)] =
                std::max(0.f, -sdlAxes[SDL_GAMEPAD_AXIS_LEFTX]);
            axes[AxisToIndex(Axis::LeftStickRight)] =
                std::max(0.f, sdlAxes[SDL_GAMEPAD_AXIS_LEFTX]);
            axes[AxisToIndex(Axis::LeftStickUp)] = std::max(0.f, -sdlAxes[SDL_GAMEPAD_AXIS_LEFTY]);
            axes[AxisToIndex(Axis::LeftStickDown)] = std::max(0.f, sdlAxes[SDL_GAMEPAD_AXIS_LEFTY]);

            axes[AxisToIndex(Axis::RightStickLeft)] =
                std::max(0.f, -sdlAxes[SDL_GAMEPAD_AXIS_RIGHTX]);
            axes[AxisToIndex(Axis::RightStickRight)] =
                std::max(0.f, sdlAxes[SDL_GAMEPAD_AXIS_RIGHTX]);
            axes[AxisToIndex(Axis::RightStickUp)] =
                std::max(0.f, -sdlAxes[SDL_GAMEPAD_AXIS_RIGHTY]);
            axes[AxisToIndex(Axis::RightStickDown)] =
                std::max(0.f, sdlAxes[SDL_GAMEPAD_AXIS_RIGHTY]);

            // Trigger axes do not require remapping since they are already between [0, 1].
            axes[AxisToIndex(Axis::LeftTrigger)] = sdlAxes[SDL_GAMEPAD_AXIS_LEFT_TRIGGER];
            axes[AxisToIndex(Axis::RightTrigger)] = sdlAxes[SDL_GAMEPAD_AXIS_RIGHT_TRIGGER];
        }
    };

    Gamepad::Gamepad(const unsigned int joystickId) : m_impl(std::make_unique<Impl>(joystickId)) {
    }

    Gamepad::~Gamepad() = default;

    unsigned int Gamepad::GetId() const noexcept {
        return SDL_GetGamepadID(m_impl->sdlGamepad);
    }

    std::string Gamepad::GetGuidString() const noexcept {
        SDL_Joystick* joy = SDL_GetGamepadJoystick(m_impl->sdlGamepad);
        if (joy == nullptr) {
            return {};
        }

        SDL_GUID guid = SDL_GetJoystickGUID(joy);
        std::array<char, 33> buf {};
        SDL_GUIDToString(guid, buf.data(), sizeof(buf));
        return std::string {buf.data()};
    }

    auto Gamepad::GetName() const noexcept -> std::string {
        const char* name = SDL_GetGamepadName(m_impl->sdlGamepad);
        return (name != nullptr) ? std::string {name} : std::string {};
    }

    auto Gamepad::GetType() const noexcept -> Gamepad::Type {
        switch (SDL_GetGamepadType(m_impl->sdlGamepad)) {
            default: return Type::Unknown;
            case SDL_GAMEPAD_TYPE_STANDARD: return Type::Standard;
            case SDL_GAMEPAD_TYPE_XBOX360: return Type::Xbox360;
            case SDL_GAMEPAD_TYPE_XBOXONE: return Type::XboxOne;
            case SDL_GAMEPAD_TYPE_PS3: return Type::Ps3;
            case SDL_GAMEPAD_TYPE_PS4: return Type::Ps4;
            case SDL_GAMEPAD_TYPE_PS5: return Type::Ps5;
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO: return Type::Switch;
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT: return Type::JoyconLeft;
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT: return Type::JoyconRight;
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR: return Type::JoyconPair;
            case SDL_GAMEPAD_TYPE_GAMECUBE: return Type::GameCube;
        }
    }

    auto Gamepad::GetButtonLabel(const Button button) const noexcept -> Gamepad::ButtonLabel {
        switch (SDL_GetGamepadButtonLabel(m_impl->sdlGamepad, Impl::ToSdlGamepadButton(button))) {
            default: return ButtonLabel::Unknown;
            case SDL_GAMEPAD_BUTTON_LABEL_A: return ButtonLabel::A;
            case SDL_GAMEPAD_BUTTON_LABEL_B: return ButtonLabel::B;
            case SDL_GAMEPAD_BUTTON_LABEL_X: return ButtonLabel::X;
            case SDL_GAMEPAD_BUTTON_LABEL_Y: return ButtonLabel::Y;
            case SDL_GAMEPAD_BUTTON_LABEL_CROSS: return ButtonLabel::Cross;
            case SDL_GAMEPAD_BUTTON_LABEL_CIRCLE: return ButtonLabel::Circle;
            case SDL_GAMEPAD_BUTTON_LABEL_SQUARE: return ButtonLabel::Square;
            case SDL_GAMEPAD_BUTTON_LABEL_TRIANGLE: return ButtonLabel::Triangle;
        }
    }

    auto Gamepad::IsButtonPressed(const Button button) const noexcept -> bool {
        return m_impl->buttonStates.IsPressed(Impl::ToSdlGamepadButton(button));
    }

    auto Gamepad::IsButtonJustPressed(const Button button) const noexcept -> bool {
        return m_impl->buttonStates.IsJustPressed(Impl::ToSdlGamepadButton(button));
    }

    auto Gamepad::IsButtonJustReleased(const Button button) const noexcept -> bool {
        return m_impl->buttonStates.IsJustReleased(Impl::ToSdlGamepadButton(button));
    }

    auto Gamepad::IsAxisPressed(const Axis axis) const noexcept -> bool {
        const std::size_t i = AxisToIndex(axis);
        return m_impl->axisStatesCurrent[i] > 0;
    }

    auto Gamepad::IsAxisJustPressed(const Axis axis) const noexcept -> bool {
        const std::size_t i = AxisToIndex(axis);
        return m_impl->axisStatesCurrent[i] > 0 && !(m_impl->axisStatesPrevious[i] > 0);
    }

    auto Gamepad::IsAxisJustReleased(const Axis axis) const noexcept -> bool {
        const std::size_t i = AxisToIndex(axis);
        return !(m_impl->axisStatesCurrent[i] > 0) && m_impl->axisStatesPrevious[i] > 0;
    }

    auto Gamepad::GetAxisStrength(const Axis axis) const noexcept -> float {
        return m_impl->axisStatesCurrent[AxisToIndex(axis)];
    }

    void Gamepad::BeginVibrate(
        const float weakPercentMagnitude,
        const float strongPercentMagnitude,
        const float durationSec
    ) const {
        constexpr std::uint16_t k_MaxMotorMagnitude {0xFFFF};
        const auto weakMagnitude = static_cast<std::uint16_t>(
            std::clamp(weakPercentMagnitude, 0.f, 1.f) * k_MaxMotorMagnitude
        );
        const auto strongMagnitude = static_cast<std::uint16_t>(
            std::clamp(strongPercentMagnitude, 0.f, 1.f) * k_MaxMotorMagnitude
        );
        const auto durationMs = static_cast<std::uint32_t>(durationSec * 1000);

        SDL_RumbleGamepad(m_impl->sdlGamepad, weakMagnitude, strongMagnitude, durationMs);
    }

    void Gamepad::StopVibrate() const {
        SDL_RumbleGamepad(m_impl->sdlGamepad, 0, 0, 0);
    }

    void Gamepad::SetLedColor(const Color color) const {
        const ColorRGBA8 rgba8 = color.ToRGBA8();
        SDL_SetGamepadLED(m_impl->sdlGamepad, rgba8.r, rgba8.g, rgba8.b);
    }

    void Gamepad::HandleEvents(const SDL_Event& event) {
    }

    void Gamepad::Update() {
        m_impl->buttonStates.Refresh();
        m_impl->sdlAxisStatesPrevious = m_impl->sdlAxisStatesCurrent;
        m_impl->axisStatesPrevious = m_impl->axisStatesCurrent;

        // Synchronize buttons with sdl buttons.
        for (std::size_t i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; i++) {
            m_impl->buttonStates.current[i] =
                SDL_GetGamepadButton(m_impl->sdlGamepad, static_cast<SDL_GamepadButton>(i));
        }

        // Normalize and store sdl axes.
        for (std::size_t i = 0; i < SDL_GAMEPAD_AXIS_COUNT; i++) {
            m_impl->sdlAxisStatesCurrent[i] = NormalizeAxis(
                SDL_GetGamepadAxis(m_impl->sdlGamepad, static_cast<SDL_GamepadAxis>(i))
            );
        }

        // Deadzone sdl axes.
        ApplyStickDeadzone(
            m_impl->sdlAxisStatesCurrent[SDL_GAMEPAD_AXIS_LEFTX],
            m_impl->sdlAxisStatesCurrent[SDL_GAMEPAD_AXIS_LEFTY],
            m_stickDeadzone
        );
        ApplyStickDeadzone(
            m_impl->sdlAxisStatesCurrent[SDL_GAMEPAD_AXIS_RIGHTX],
            m_impl->sdlAxisStatesCurrent[SDL_GAMEPAD_AXIS_RIGHTY],
            m_stickDeadzone
        );
        ApplyTriggerDeadzone(
            m_impl->sdlAxisStatesCurrent[SDL_GAMEPAD_AXIS_LEFT_TRIGGER], m_triggerDeadzone
        );
        ApplyTriggerDeadzone(
            m_impl->sdlAxisStatesCurrent[SDL_GAMEPAD_AXIS_RIGHT_TRIGGER], m_triggerDeadzone
        );

        // Remap sdl axes to axes.
        Impl::RemapSdlAxes(m_impl->axisStatesCurrent, m_impl->sdlAxisStatesCurrent);
    }
} // namespace vn