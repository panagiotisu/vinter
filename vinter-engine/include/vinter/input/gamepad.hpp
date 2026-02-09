#pragma once

#include <memory>
#include <string>

#include "vinter/input/buttons.hpp"

namespace vn {
    enum class GamepadButton {
        // Common Buttons.
        South, East, West, North,
        Back, Guide, Start,
        LeftStick, RightStick, LeftShoulder, RightShoulder,
        DpadUp, DpadDown, DpadLeft, DpadRight,

        // Specialized Buttons that depend on gamepad company and model.
        RightPaddle1,
        LeftPaddle1,
        RightPaddle2,
        LeftPaddle2,
        Touchpad,
        Misc1, /* Additional button (e.g. Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro
                  capture button, Amazon Luna microphone button, Google Stadia capture button). */
        Misc2,
        Misc3,  // Additional button (e.g. Nintendo GameCube left trigger click).
        Misc4,  // Additional button (e.g. Nintendo GameCube right trigger click).
        Misc5,
        Misc6,
    };

    enum class GamepadButtonLabel {
        Unknown,
        A, B, X, Y,
        Cross, Circle, Square, Triangle,
    };

    enum class GamepadType {
        Unknown,
        Standard,
        Xbox360, XboxOne,
        Ps3, Ps4, Ps5,
        Switch, JoyconLeft, JoyconRight, JoyconPair,
        GameCube,
    };

    class Gamepad {
        friend class Engine;
    public:
        Gamepad();
        ~Gamepad();

        [[nodiscard]] unsigned int get_id() const noexcept;
        [[nodiscard]] std::string get_name() const noexcept;
        [[nodiscard]] GamepadType get_type() const noexcept;
        [[nodiscard]] GamepadButtonLabel get_button_label(GamepadButton button) const noexcept;

        [[nodiscard]] bool is_button_pressed(GamepadButton button) const noexcept;
        [[nodiscard]] bool is_button_just_pressed(GamepadButton button) const noexcept;
        [[nodiscard]] bool is_button_just_released(GamepadButton button) const noexcept;

    private:
        void update();

        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // vn