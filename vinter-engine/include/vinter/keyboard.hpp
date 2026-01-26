#pragma once

#include <memory>

namespace vn {
    enum class Key {
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        _1, _2, _3, _4, _5, _6, _7, _8, _9, _0,
        Q, W, E, R, T, Y, U, I, O, P, A, S, D, F, G, H, J, K, L, Z, X, C, V, B, N, M,
        Esc, Space, Backspace, Enter,
        Up, Down, Left, Right,
    };

    class Keyboard {
        friend class Engine;

    public:
        Keyboard();
        ~Keyboard();

        [[nodiscard]] bool is_key_pressed(Key key) const;
        [[nodiscard]] bool is_key_up(Key key) const;
        [[nodiscard]] bool is_key_just_pressed(Key key) const;
        [[nodiscard]] bool is_key_just_released(Key key) const;

    private:
        void update();

        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // vn