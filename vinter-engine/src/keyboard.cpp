#include "vinter/keyboard.hpp"

#include <SDL3/SDL.h>

#include <array>

namespace vn {
    struct Keyboard::Impl {
        const bool* current { SDL_GetKeyboardState(nullptr) };
        std::array<bool, SDL_SCANCODE_COUNT> previous {};

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

                // Letters (QWERTY order)
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

                // Control keys
                case Key::Esc:       return SDL_SCANCODE_ESCAPE;
                case Key::Space:     return SDL_SCANCODE_SPACE;
                case Key::Backspace: return SDL_SCANCODE_BACKSPACE;
                case Key::Enter:     return SDL_SCANCODE_RETURN;

                // Arrows
                case Key::Up:    return SDL_SCANCODE_UP;
                case Key::Down:  return SDL_SCANCODE_DOWN;
                case Key::Left:  return SDL_SCANCODE_LEFT;
                case Key::Right: return SDL_SCANCODE_RIGHT;
            }
        }
    };

    Keyboard::Keyboard()
        : m_impl(std::make_unique<Impl>()) {
    }

    Keyboard::~Keyboard() = default;

    bool Keyboard::is_key_pressed(const Key key) const {
        const SDL_Scancode sdl_key = Impl::to_sdl_scancode(key);
        return m_impl->current[sdl_key];
    }
    bool Keyboard::is_key_released(const Key key) const {
        const SDL_Scancode sdl_key = Impl::to_sdl_scancode(key);
        return !m_impl->current[sdl_key];
    }
    bool Keyboard::is_key_just_pressed(const Key key) const {
        const SDL_Scancode sdl_key = Impl::to_sdl_scancode(key);
        return m_impl->current[sdl_key] && !m_impl->previous[sdl_key];
    }
    bool Keyboard::is_key_just_released(const Key key) const {
        const SDL_Scancode sdl_key = Impl::to_sdl_scancode(key);
        return !m_impl->current[sdl_key] && m_impl->previous[sdl_key];
    }

    void Keyboard::update() {
        for (std::size_t i = 0; i < SDL_SCANCODE_COUNT; i++) {
            m_impl->previous[i] = m_impl->current[i];
        }
    }
} // vn