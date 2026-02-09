#include "vinter/input/gamepad.hpp"

#include <SDL3/SDL.h>

#include "vinter/input/buttons.hpp"

namespace vn {
    struct Gamepad::Impl {
        SDL_Gamepad* sdl_gamepad { nullptr };
        Buttons<SDL_GAMEPAD_BUTTON_COUNT> buttons {};
        std::array<float, SDL_GAMEPAD_AXIS_COUNT> axes_current {};
        std::array<float, SDL_GAMEPAD_AXIS_COUNT> axes_previous {};

        explicit Impl(const unsigned int joystick_id)
            : sdl_gamepad(SDL_OpenGamepad(joystick_id)) {
        }

        ~Impl() {
            SDL_CloseGamepad(sdl_gamepad);
        }

        static SDL_GamepadButton to_sdl_gamepad_button(const Button button) noexcept {
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
        return std::string { SDL_GetGamepadName(m_impl->sdl_gamepad) };
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

    bool Gamepad::is_button_pressed(const Button button) const noexcept {
        return m_impl->buttons.is_pressed(Impl::to_sdl_gamepad_button(button));
    }
    bool Gamepad::is_button_just_pressed(const Button button) const noexcept {
        return m_impl->buttons.is_just_pressed(Impl::to_sdl_gamepad_button(button));
    }
    bool Gamepad::is_button_just_released(const Button button) const noexcept {
        return m_impl->buttons.is_just_released(Impl::to_sdl_gamepad_button(button));
    }

    void Gamepad::handle_events(const SDL_Event& event) {
    }

    void Gamepad::update() {
        m_impl->buttons.update();

        // Synchronize with actual sdl state.
        for (std::size_t i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; i++) {
            m_impl->buttons.state_current[i] = SDL_GetGamepadButton(m_impl->sdl_gamepad, static_cast<SDL_GamepadButton>(i));
        }
    }
} // vn