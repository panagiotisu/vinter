#include "vinter/input/gamepad.hpp"

#include <algorithm>
#include <cmath>
#include <array>
#include <cassert>

#include <SDL3/SDL.h>

#include "vinter/color.hpp"
#include "vinter/input/button_states.hpp"

namespace vn {
    static float normalize_axis(const float axis) noexcept {
        if (axis < 0.f) return -axis / SDL_JOYSTICK_AXIS_MIN;
        return axis / SDL_JOYSTICK_AXIS_MAX;
    }

    static void apply_trigger_deadzone(float& trigger_value, const float deadzone) {
        if (trigger_value < deadzone) {
            trigger_value = 0.f;
        } else {
            trigger_value = (trigger_value - deadzone) / (1.f - deadzone);
        }
    }

    static void apply_stick_deadzone(float& stick_x, float& stick_y, const float deadzone) {
        const float magnitude_squared = stick_x * stick_x + stick_y * stick_y;
        if (magnitude_squared < deadzone * deadzone) {
            stick_x = 0.f;
            stick_y = 0.f;
        } else {
            const float magnitude = std::sqrt(magnitude_squared);
            const float magnitude_scaled = (magnitude - deadzone) / (1.f - deadzone);

            stick_x *= magnitude_scaled / magnitude;
            stick_y *= magnitude_scaled / magnitude;
        }
    }

    static std::size_t axis_to_index(const Gamepad::Axis axis) {
        return static_cast<std::size_t>(axis);
    }

    struct Gamepad::Impl {
        SDL_Gamepad* sdl_gamepad { nullptr };
        ButtonStates<SDL_GAMEPAD_BUTTON_COUNT> button_states {};
        std::array<float, SDL_GAMEPAD_AXIS_COUNT> sdl_axis_states_current {}, sdl_axis_states_previous {};
        std::array<float, static_cast<std::size_t>(Axis::Count)> axis_states_current {}, axis_states_previous {};

        explicit Impl(const unsigned int joystick_id) {
            sdl_gamepad = SDL_OpenGamepad(joystick_id);
            assert(sdl_gamepad && "Failed to open SDL gamepad.");
        }

        ~Impl() {
            if (sdl_gamepad) SDL_CloseGamepad(sdl_gamepad);
        }

        [[nodiscard]] static SDL_GamepadButton to_sdl_gamepad_button(const Button button) noexcept {
            switch (button) {
                default: return SDL_GAMEPAD_BUTTON_INVALID;

                case Button::South:          return SDL_GAMEPAD_BUTTON_SOUTH;
                case Button::East:           return SDL_GAMEPAD_BUTTON_EAST;
                case Button::West:           return SDL_GAMEPAD_BUTTON_WEST;
                case Button::North:          return SDL_GAMEPAD_BUTTON_NORTH;

                case Button::Back:           return SDL_GAMEPAD_BUTTON_BACK;
                case Button::Guide:          return SDL_GAMEPAD_BUTTON_GUIDE;
                case Button::Start:          return SDL_GAMEPAD_BUTTON_START;

                case Button::LeftStick:      return SDL_GAMEPAD_BUTTON_LEFT_STICK;
                case Button::RightStick:     return SDL_GAMEPAD_BUTTON_RIGHT_STICK;

                case Button::LeftShoulder:   return SDL_GAMEPAD_BUTTON_LEFT_SHOULDER;
                case Button::RightShoulder:  return SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER;

                case Button::DpadUp:          return SDL_GAMEPAD_BUTTON_DPAD_UP;
                case Button::DpadDown:        return SDL_GAMEPAD_BUTTON_DPAD_DOWN;
                case Button::DpadLeft:        return SDL_GAMEPAD_BUTTON_DPAD_LEFT;
                case Button::DpadRight:       return SDL_GAMEPAD_BUTTON_DPAD_RIGHT;

                case Button::RightPaddle1:    return SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1;
                case Button::LeftPaddle1:     return SDL_GAMEPAD_BUTTON_LEFT_PADDLE1;
                case Button::RightPaddle2:    return SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2;
                case Button::LeftPaddle2:     return SDL_GAMEPAD_BUTTON_LEFT_PADDLE2;

                case Button::Touchpad:        return SDL_GAMEPAD_BUTTON_TOUCHPAD;

                case Button::Misc1:            return SDL_GAMEPAD_BUTTON_MISC1;
                case Button::Misc2:            return SDL_GAMEPAD_BUTTON_MISC2;
                case Button::Misc3:            return SDL_GAMEPAD_BUTTON_MISC3;
                case Button::Misc4:            return SDL_GAMEPAD_BUTTON_MISC4;
                case Button::Misc5:            return SDL_GAMEPAD_BUTTON_MISC5;
                case Button::Misc6:            return SDL_GAMEPAD_BUTTON_MISC6;
            }
        }

        static void remap_sdl_axes(
            std::array<float, static_cast<std::size_t>(Axis::Count)>& axes,
            const std::array<float, SDL_GAMEPAD_AXIS_COUNT>& sdl_axes
        ) {
            // Split and remap stick axes so that they are always between [0, 1] instead of [-1, 1].
            axes[axis_to_index(Axis::LeftStickLeft)]  = std::max(0.f, -sdl_axes[SDL_GAMEPAD_AXIS_LEFTX]);
            axes[axis_to_index(Axis::LeftStickRight)] = std::max(0.f,  sdl_axes[SDL_GAMEPAD_AXIS_LEFTX]);
            axes[axis_to_index(Axis::LeftStickUp)]    = std::max(0.f, -sdl_axes[SDL_GAMEPAD_AXIS_LEFTY]);
            axes[axis_to_index(Axis::LeftStickDown)]  = std::max(0.f,  sdl_axes[SDL_GAMEPAD_AXIS_LEFTY]);

            axes[axis_to_index(Axis::RightStickLeft)]  = std::max(0.f, -sdl_axes[SDL_GAMEPAD_AXIS_RIGHTX]);
            axes[axis_to_index(Axis::RightStickRight)] = std::max(0.f,  sdl_axes[SDL_GAMEPAD_AXIS_RIGHTX]);
            axes[axis_to_index(Axis::RightStickUp)]    = std::max(0.f, -sdl_axes[SDL_GAMEPAD_AXIS_RIGHTY]);
            axes[axis_to_index(Axis::RightStickDown)]  = std::max(0.f,  sdl_axes[SDL_GAMEPAD_AXIS_RIGHTY]);

            // Trigger axes do not require remapping since they are already between [0, 1].
            axes[axis_to_index(Axis::LeftTrigger)]  = sdl_axes[SDL_GAMEPAD_AXIS_LEFT_TRIGGER];
            axes[axis_to_index(Axis::RightTrigger)] = sdl_axes[SDL_GAMEPAD_AXIS_RIGHT_TRIGGER];
        }
    };

    Gamepad::Gamepad(const unsigned int joystick_id)
        : m_impl(std::make_unique<Impl>(joystick_id)) {
    }

    Gamepad::~Gamepad() = default;

    unsigned int Gamepad::get_id() const noexcept {
        return SDL_GetGamepadID(m_impl->sdl_gamepad);
    }

    std::string Gamepad::get_guid_string() const noexcept {
        SDL_Joystick* joy = SDL_GetGamepadJoystick(m_impl->sdl_gamepad);
        if (!joy) return {};

        SDL_GUID guid = SDL_GetJoystickGUID(joy);
        char buf[33];
        SDL_GUIDToString(guid, buf, sizeof(buf));
        return std::string { buf };
    }

    std::string Gamepad::get_name() const noexcept {
        const char* name = SDL_GetGamepadName(m_impl->sdl_gamepad);
        return name ? std::string{name} : std::string{};
    }

    Gamepad::Type Gamepad::get_type() const noexcept {
        switch (SDL_GetGamepadType(m_impl->sdl_gamepad)) {
            default: return Type::Unknown;
            case SDL_GAMEPAD_TYPE_STANDARD:                     return Type::Standard;
            case SDL_GAMEPAD_TYPE_XBOX360:                      return Type::Xbox360;
            case SDL_GAMEPAD_TYPE_XBOXONE:                      return Type::XboxOne;
            case SDL_GAMEPAD_TYPE_PS3:                          return Type::Ps3;
            case SDL_GAMEPAD_TYPE_PS4:                          return Type::Ps4;
            case SDL_GAMEPAD_TYPE_PS5:                          return Type::Ps5;
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO:          return Type::Switch;
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:  return Type::JoyconLeft;
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT: return Type::JoyconRight;
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR:  return Type::JoyconPair;
            case SDL_GAMEPAD_TYPE_GAMECUBE:                     return Type::GameCube;
        }
    }

    Gamepad::ButtonLabel Gamepad::get_button_label(const Button button) const noexcept {
        switch (SDL_GetGamepadButtonLabel(m_impl->sdl_gamepad, Impl::to_sdl_gamepad_button(button))) {
            default: return ButtonLabel::Unknown;
            case SDL_GAMEPAD_BUTTON_LABEL_A:        return ButtonLabel::A;
            case SDL_GAMEPAD_BUTTON_LABEL_B:        return ButtonLabel::B;
            case SDL_GAMEPAD_BUTTON_LABEL_X:        return ButtonLabel::X;
            case SDL_GAMEPAD_BUTTON_LABEL_Y:        return ButtonLabel::Y;
            case SDL_GAMEPAD_BUTTON_LABEL_CROSS:    return ButtonLabel::Cross;
            case SDL_GAMEPAD_BUTTON_LABEL_CIRCLE:   return ButtonLabel::Circle;
            case SDL_GAMEPAD_BUTTON_LABEL_SQUARE:   return ButtonLabel::Square;
            case SDL_GAMEPAD_BUTTON_LABEL_TRIANGLE: return ButtonLabel::Triangle;
        }
    }

    bool Gamepad::is_button_pressed(const Button button) const noexcept {
        return m_impl->button_states.is_pressed(Impl::to_sdl_gamepad_button(button));
    }
    bool Gamepad::is_button_just_pressed(const Button button) const noexcept {
        return m_impl->button_states.is_just_pressed(Impl::to_sdl_gamepad_button(button));
    }
    bool Gamepad::is_button_just_released(const Button button) const noexcept {
        return m_impl->button_states.is_just_released(Impl::to_sdl_gamepad_button(button));
    }
    bool Gamepad::is_axis_pressed(const Axis axis) const noexcept {
        const std::size_t i = axis_to_index(axis);
        return m_impl->axis_states_current[i] > 0;
    }
    bool Gamepad::is_axis_just_pressed(const Axis axis) const noexcept {
        const std::size_t i = axis_to_index(axis);
        return m_impl->axis_states_current[i] > 0 && !(m_impl->axis_states_previous[i] > 0);
    }
    bool Gamepad::is_axis_just_released(const Axis axis) const noexcept {
        const std::size_t i = axis_to_index(axis);
        return !(m_impl->axis_states_current[i] > 0) && m_impl->axis_states_previous[i] > 0;
    }

    float Gamepad::get_axis_strength(const Axis axis) const noexcept {
        return m_impl->axis_states_current[axis_to_index(axis)];
    }

    void Gamepad::begin_vibrate(
        const float weak_percent_magnitude,
        const float strong_percent_magnitude,
        const float duration_sec
    ) const {
        constexpr std::uint16_t max_motor_magnitude { 0xFFFF };
        const auto weak_magnitude = static_cast<std::uint16_t>(std::clamp(weak_percent_magnitude, 0.f, 1.f) * max_motor_magnitude);
        const auto strong_magnitude = static_cast<std::uint16_t>(std::clamp(strong_percent_magnitude, 0.f, 1.f) * max_motor_magnitude);
        const auto duration_ms = static_cast<std::uint32_t>(duration_sec * 1000);

        SDL_RumbleGamepad(
            m_impl->sdl_gamepad,
            weak_magnitude,
            strong_magnitude,
            duration_ms
        );
    }

    void Gamepad::stop_vibrate() const {
        SDL_RumbleGamepad(m_impl->sdl_gamepad, 0, 0, 0);
    }

    void Gamepad::set_led_color(const Color color) const {
        SDL_SetGamepadLED(m_impl->sdl_gamepad, color.r, color.g, color.b);
    }

    void Gamepad::handle_events(const SDL_Event& event) {
    }

    void Gamepad::update() {
        m_impl->button_states.refresh();
        m_impl->sdl_axis_states_previous = m_impl->sdl_axis_states_current;
        m_impl->axis_states_previous = m_impl->axis_states_current;

        // Synchronize buttons with sdl buttons.
        for (std::size_t i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; i++) {
            m_impl->button_states.current[i] = SDL_GetGamepadButton(
                m_impl->sdl_gamepad,
                static_cast<SDL_GamepadButton>(i)
            );
        }

        // Normalize and store sdl axes.
        for (std::size_t i = 0; i < SDL_GAMEPAD_AXIS_COUNT; i++) {
            m_impl->sdl_axis_states_current[i] = normalize_axis(SDL_GetGamepadAxis(
                m_impl->sdl_gamepad,
                static_cast<SDL_GamepadAxis>(i)
            ));
        }

        // Deadzone sdl axes.
        apply_stick_deadzone(
            m_impl->sdl_axis_states_current[SDL_GAMEPAD_AXIS_LEFTX],
            m_impl->sdl_axis_states_current[SDL_GAMEPAD_AXIS_LEFTY],
            m_stick_deadzone
        );
        apply_stick_deadzone(
            m_impl->sdl_axis_states_current[SDL_GAMEPAD_AXIS_RIGHTX],
            m_impl->sdl_axis_states_current[SDL_GAMEPAD_AXIS_RIGHTY],
            m_stick_deadzone
        );
        apply_trigger_deadzone(m_impl->sdl_axis_states_current[SDL_GAMEPAD_AXIS_LEFT_TRIGGER], m_trigger_deadzone);
        apply_trigger_deadzone(m_impl->sdl_axis_states_current[SDL_GAMEPAD_AXIS_RIGHT_TRIGGER], m_trigger_deadzone);

        // Remap sdl axes to axes.
        Impl::remap_sdl_axes(m_impl->axis_states_current, m_impl->sdl_axis_states_current);
    }
} // vn