#include "vinter/input/keyboard.hpp"

#include <SDL3/SDL.h>

#include "buttons.hpp"

namespace vn {
    struct Keyboard::Impl {
        const bool* sdl_state { SDL_GetKeyboardState(nullptr) };
        Buttons<SDL_SCANCODE_COUNT> keys;

        static SDL_Scancode to_sdl_scancode(const Key key) {
            switch (key) {
                default: return SDL_SCANCODE_UNKNOWN;

                // Function keys
                case Key::F1:  return SDL_SCANCODE_F1;
                case Key::F2:  return SDL_SCANCODE_F2;
                case Key::F3:  return SDL_SCANCODE_F3;
                case Key::F4:  return SDL_SCANCODE_F4;
                case Key::F5:  return SDL_SCANCODE_F5;
                case Key::F6:  return SDL_SCANCODE_F6;
                case Key::F7:  return SDL_SCANCODE_F7;
                case Key::F8:  return SDL_SCANCODE_F8;
                case Key::F9:  return SDL_SCANCODE_F9;
                case Key::F10: return SDL_SCANCODE_F10;
                case Key::F11: return SDL_SCANCODE_F11;
                case Key::F12: return SDL_SCANCODE_F12;

                // Number row
                case Key::_1: return SDL_SCANCODE_1;
                case Key::_2: return SDL_SCANCODE_2;
                case Key::_3: return SDL_SCANCODE_3;
                case Key::_4: return SDL_SCANCODE_4;
                case Key::_5: return SDL_SCANCODE_5;
                case Key::_6: return SDL_SCANCODE_6;
                case Key::_7: return SDL_SCANCODE_7;
                case Key::_8: return SDL_SCANCODE_8;
                case Key::_9: return SDL_SCANCODE_9;
                case Key::_0: return SDL_SCANCODE_0;

                // Letters
                case Key::Q: return SDL_SCANCODE_Q;
                case Key::W: return SDL_SCANCODE_W;
                case Key::E: return SDL_SCANCODE_E;
                case Key::R: return SDL_SCANCODE_R;
                case Key::T: return SDL_SCANCODE_T;
                case Key::Y: return SDL_SCANCODE_Y;
                case Key::U: return SDL_SCANCODE_U;
                case Key::I: return SDL_SCANCODE_I;
                case Key::O: return SDL_SCANCODE_O;
                case Key::P: return SDL_SCANCODE_P;
                case Key::A: return SDL_SCANCODE_A;
                case Key::S: return SDL_SCANCODE_S;
                case Key::D: return SDL_SCANCODE_D;
                case Key::F: return SDL_SCANCODE_F;
                case Key::G: return SDL_SCANCODE_G;
                case Key::H: return SDL_SCANCODE_H;
                case Key::J: return SDL_SCANCODE_J;
                case Key::K: return SDL_SCANCODE_K;
                case Key::L: return SDL_SCANCODE_L;
                case Key::Z: return SDL_SCANCODE_Z;
                case Key::X: return SDL_SCANCODE_X;
                case Key::C: return SDL_SCANCODE_C;
                case Key::V: return SDL_SCANCODE_V;
                case Key::B: return SDL_SCANCODE_B;
                case Key::N: return SDL_SCANCODE_N;
                case Key::M: return SDL_SCANCODE_M;

                // Special keys
                case Key::Esc:       return SDL_SCANCODE_ESCAPE;
                case Key::Tab:       return SDL_SCANCODE_TAB;
                case Key::CapsLock:  return SDL_SCANCODE_CAPSLOCK;
                case Key::Space:     return SDL_SCANCODE_SPACE;
                case Key::Enter:     return SDL_SCANCODE_RETURN;
                case Key::Backspace: return SDL_SCANCODE_BACKSPACE;
                case Key::Insert:    return SDL_SCANCODE_INSERT;
                case Key::Delete:    return SDL_SCANCODE_DELETE;
                case Key::Home:      return SDL_SCANCODE_HOME;
                case Key::End:       return SDL_SCANCODE_END;
                case Key::PageUp:    return SDL_SCANCODE_PAGEUP;
                case Key::PageDown:  return SDL_SCANCODE_PAGEDOWN;

                // Arrows
                case Key::Up:    return SDL_SCANCODE_UP;
                case Key::Down:  return SDL_SCANCODE_DOWN;
                case Key::Left:  return SDL_SCANCODE_LEFT;
                case Key::Right: return SDL_SCANCODE_RIGHT;

                // Symbols/Punctuation
                case Key::Minus:        return SDL_SCANCODE_MINUS;
                case Key::Equals:       return SDL_SCANCODE_EQUALS;
                case Key::LeftBracket:  return SDL_SCANCODE_LEFTBRACKET;
                case Key::RightBracket: return SDL_SCANCODE_RIGHTBRACKET;
                case Key::Semicolon:    return SDL_SCANCODE_SEMICOLON;
                case Key::Apostrophe:   return SDL_SCANCODE_APOSTROPHE;
                case Key::Grave:        return SDL_SCANCODE_GRAVE;
                case Key::Backslash:    return SDL_SCANCODE_BACKSLASH;
                case Key::Comma:        return SDL_SCANCODE_COMMA;
                case Key::Period:       return SDL_SCANCODE_PERIOD;
                case Key::Slash:        return SDL_SCANCODE_SLASH;

                // Numpad
                case Key::Numpad0: return SDL_SCANCODE_KP_0;
                case Key::Numpad1: return SDL_SCANCODE_KP_1;
                case Key::Numpad2: return SDL_SCANCODE_KP_2;
                case Key::Numpad3: return SDL_SCANCODE_KP_3;
                case Key::Numpad4: return SDL_SCANCODE_KP_4;
                case Key::Numpad5: return SDL_SCANCODE_KP_5;
                case Key::Numpad6: return SDL_SCANCODE_KP_6;
                case Key::Numpad7: return SDL_SCANCODE_KP_7;
                case Key::Numpad8: return SDL_SCANCODE_KP_8;
                case Key::Numpad9: return SDL_SCANCODE_KP_9;
                case Key::NumpadMultiply: return SDL_SCANCODE_KP_MULTIPLY;
                case Key::NumpadDivide:   return SDL_SCANCODE_KP_DIVIDE;
                case Key::NumpadPlus:     return SDL_SCANCODE_KP_PLUS;
                case Key::NumpadMinus:    return SDL_SCANCODE_KP_MINUS;
                case Key::NumpadEnter:    return SDL_SCANCODE_KP_ENTER;
                case Key::NumpadPeriod:   return SDL_SCANCODE_KP_PERIOD;
                case Key::NumLock:        return SDL_SCANCODE_NUMLOCKCLEAR;

                // Other
                case Key::PrintScreen: return SDL_SCANCODE_PRINTSCREEN;
                case Key::ScrollLock:  return SDL_SCANCODE_SCROLLLOCK;
                case Key::Pause:       return SDL_SCANCODE_PAUSE;
            }
        }
    };

    Keyboard::Keyboard()
        : m_impl(std::make_unique<Impl>()) {
    }

    Keyboard::~Keyboard() = default;

    bool Keyboard::is_key_pressed(const Key key) const {
        return m_impl->keys.is_pressed(Impl::to_sdl_scancode(key));
    }
    bool Keyboard::is_key_just_pressed(const Key key) const {
        return m_impl->keys.is_just_pressed(Impl::to_sdl_scancode(key));
    }
    bool Keyboard::is_key_just_released(const Key key) const {
        return m_impl->keys.is_just_released(Impl::to_sdl_scancode(key));
    }

    void Keyboard::update() {
        m_impl->keys.update();

        // Synchronize with actual sdl state.
        for (std::size_t i = 0; i < SDL_SCANCODE_COUNT; i++) {
            m_impl->keys.state_current[i] = m_impl->sdl_state[i];
        }
    }
} // vn