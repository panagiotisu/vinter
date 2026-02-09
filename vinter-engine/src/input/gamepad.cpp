#include "vinter/input/gamepad.hpp"

#include <SDL3/SDL.h>

namespace vn {
    struct Gamepad::Impl {
        SDL_Gamepad* sdl_gamepad { nullptr };
        Buttons<SDL_GAMEPAD_BUTTON_COUNT> buttons {};

        static SDL_GamepadButton to_sdl_gamepad_button(const GamepadButton button) noexcept {
            switch (button) {
                default: return SDL_GAMEPAD_BUTTON_INVALID;

                case GamepadButton::South:          return SDL_GAMEPAD_BUTTON_SOUTH;
                case GamepadButton::East:           return SDL_GAMEPAD_BUTTON_EAST;
                case GamepadButton::West:           return SDL_GAMEPAD_BUTTON_WEST;
                case GamepadButton::North:          return SDL_GAMEPAD_BUTTON_NORTH;

                case GamepadButton::Back:           return SDL_GAMEPAD_BUTTON_BACK;
                case GamepadButton::Guide:          return SDL_GAMEPAD_BUTTON_GUIDE;
                case GamepadButton::Start:          return SDL_GAMEPAD_BUTTON_START;

                case GamepadButton::LeftStick:      return SDL_GAMEPAD_BUTTON_LEFT_STICK;
                case GamepadButton::RightStick:     return SDL_GAMEPAD_BUTTON_RIGHT_STICK;

                case GamepadButton::LeftShoulder:   return SDL_GAMEPAD_BUTTON_LEFT_SHOULDER;
                case GamepadButton::RightShoulder:  return SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER;

                case GamepadButton::DpadUp:          return SDL_GAMEPAD_BUTTON_DPAD_UP;
                case GamepadButton::DpadDown:        return SDL_GAMEPAD_BUTTON_DPAD_DOWN;
                case GamepadButton::DpadLeft:        return SDL_GAMEPAD_BUTTON_DPAD_LEFT;
                case GamepadButton::DpadRight:       return SDL_GAMEPAD_BUTTON_DPAD_RIGHT;

                case GamepadButton::RightPaddle1:    return SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1;
                case GamepadButton::LeftPaddle1:     return SDL_GAMEPAD_BUTTON_LEFT_PADDLE1;
                case GamepadButton::RightPaddle2:    return SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2;
                case GamepadButton::LeftPaddle2:     return SDL_GAMEPAD_BUTTON_LEFT_PADDLE2;

                case GamepadButton::Touchpad:        return SDL_GAMEPAD_BUTTON_TOUCHPAD;

                case GamepadButton::Misc1:            return SDL_GAMEPAD_BUTTON_MISC1;
                case GamepadButton::Misc2:            return SDL_GAMEPAD_BUTTON_MISC2;
                case GamepadButton::Misc3:            return SDL_GAMEPAD_BUTTON_MISC3;
                case GamepadButton::Misc4:            return SDL_GAMEPAD_BUTTON_MISC4;
                case GamepadButton::Misc5:            return SDL_GAMEPAD_BUTTON_MISC5;
                case GamepadButton::Misc6:            return SDL_GAMEPAD_BUTTON_MISC6;
            }
        }
    };

    Gamepad::Gamepad()
        : m_impl(std::make_unique<Impl>()) {
        m_impl->sdl_gamepad = SDL_OpenGamepad(0);
    }

    Gamepad::~Gamepad() {
        SDL_CloseGamepad(m_impl->sdl_gamepad);
    }

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

    GamepadType Gamepad::get_type() const noexcept {
        switch (SDL_GetGamepadType(m_impl->sdl_gamepad)) {
            default: return GamepadType::Unknown;
            case SDL_GAMEPAD_TYPE_STANDARD:                     return GamepadType::Standard;
            case SDL_GAMEPAD_TYPE_XBOX360:                      return GamepadType::Xbox360;
            case SDL_GAMEPAD_TYPE_XBOXONE:                      return GamepadType::XboxOne;
            case SDL_GAMEPAD_TYPE_PS3:                          return GamepadType::Ps3;
            case SDL_GAMEPAD_TYPE_PS4:                          return GamepadType::Ps4;
            case SDL_GAMEPAD_TYPE_PS5:                          return GamepadType::Ps5;
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO:          return GamepadType::Switch;
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:  return GamepadType::JoyconLeft;
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT: return GamepadType::JoyconRight;
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR:  return GamepadType::JoyconPair;
            case SDL_GAMEPAD_TYPE_GAMECUBE:                     return GamepadType::GameCube;
        }
    }

    GamepadButtonLabel Gamepad::get_button_label(const GamepadButton button) const noexcept {
        switch (SDL_GetGamepadButtonLabel(m_impl->sdl_gamepad, Impl::to_sdl_gamepad_button(button))) {
            default: return GamepadButtonLabel::Unknown;
            case SDL_GAMEPAD_BUTTON_LABEL_A: return GamepadButtonLabel::A;
            case SDL_GAMEPAD_BUTTON_LABEL_B: return GamepadButtonLabel::B;
            case SDL_GAMEPAD_BUTTON_LABEL_X: return GamepadButtonLabel::X;
            case SDL_GAMEPAD_BUTTON_LABEL_Y: return GamepadButtonLabel::Y;
            case SDL_GAMEPAD_BUTTON_LABEL_CROSS: return GamepadButtonLabel::Cross;
            case SDL_GAMEPAD_BUTTON_LABEL_CIRCLE: return GamepadButtonLabel::Circle;
            case SDL_GAMEPAD_BUTTON_LABEL_SQUARE: return GamepadButtonLabel::Square;
            case SDL_GAMEPAD_BUTTON_LABEL_TRIANGLE: return GamepadButtonLabel::Triangle;
        }
    }

    bool Gamepad::is_button_pressed(const GamepadButton button) const noexcept {
        return m_impl->buttons.is_pressed(Impl::to_sdl_gamepad_button(button));
    }
    bool Gamepad::is_button_just_pressed(const GamepadButton button) const noexcept {
        return m_impl->buttons.is_just_pressed(Impl::to_sdl_gamepad_button(button));
    }
    bool Gamepad::is_button_just_released(const GamepadButton button) const noexcept {
        return m_impl->buttons.is_just_released(Impl::to_sdl_gamepad_button(button));
    }

    void Gamepad::update() {
        m_impl->buttons.update();

        // Synchronize with actual sdl state.
        for (std::size_t i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; i++) {
            m_impl->buttons.state_current[i] = SDL_GetGamepadButton(m_impl->sdl_gamepad, static_cast<SDL_GamepadButton>(i));
        }
    }
} // vn