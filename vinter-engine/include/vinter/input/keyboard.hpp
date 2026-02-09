#pragma once

#include <memory>

namespace vn {
    class Keyboard {
        friend class Engine;

    public:
        Keyboard();
        ~Keyboard();

        enum class Key {
            // Function keys
            F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

            // Number row
            _1, _2, _3, _4, _5, _6, _7, _8, _9, _0,

            // Letters
            Q, W, E, R, T, Y, U, I, O, P,
            A, S, D, F, G, H, J, K, L,
            Z, X, C, V, B, N, M,

            // Special keys
            Esc, Tab, CapsLock,
            Space, Enter, Backspace,
            Insert, Delete, Home, End, PageUp, PageDown,

            // Arrow keys
            Up, Down, Left, Right,

            // Symbols/Punctuation
            Minus, Equals,
            LeftBracket, RightBracket,
            Semicolon, Apostrophe,
            Grave, Backslash,
            Comma, Period, Slash,

            // Numpad
            Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
            Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
            NumpadMultiply, NumpadDivide, NumpadPlus, NumpadMinus,
            NumpadEnter, NumpadPeriod,
            NumLock,

            // Other
            PrintScreen, ScrollLock, Pause,
        };

        [[nodiscard]] bool is_key_pressed(Key key) const;
        [[nodiscard]] bool is_key_just_pressed(Key key) const;
        [[nodiscard]] bool is_key_just_released(Key key) const;

    private:
        void update();

        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // vn