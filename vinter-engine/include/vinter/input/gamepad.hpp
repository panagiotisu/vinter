#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

union SDL_Event;

namespace vn {
    class Gamepad {
        friend class DeviceManager;

    public:
        explicit Gamepad(unsigned int joystick_id);
        ~Gamepad();

        enum class Button {
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

        enum class Axis {
            LeftStickLeft, LeftStickRight, LeftStickUp, LeftStickDown,
            RightStickLeft, RightStickRight, RightStickUp, RightStickDown,
            LeftTrigger, RightTrigger,
        };

        enum class ButtonLabel {
            Unknown,
            A, B, X, Y,
            Cross, Circle, Square, Triangle,
        };

        enum class Type {
            Unknown,
            Standard,
            Xbox360, XboxOne,
            Ps3, Ps4, Ps5,
            Switch, JoyconLeft, JoyconRight, JoyconPair,
            GameCube,
        };

        [[nodiscard]] unsigned int get_id() const noexcept;
        [[nodiscard]] std::string get_guid_string() const noexcept;
        [[nodiscard]] std::string get_name() const noexcept;
        [[nodiscard]] Type get_type() const noexcept;
        [[nodiscard]] ButtonLabel get_button_label(Button button) const noexcept;

        [[nodiscard]] bool is_button_pressed(Button button) const noexcept;
        [[nodiscard]] bool is_button_just_pressed(Button button) const noexcept;
        [[nodiscard]] bool is_button_just_released(Button button) const noexcept;
        [[nodiscard]] float get_axis_strength(Axis axis) const noexcept;

    private:
        void handle_events(const SDL_Event& event);
        void update();

        // Caches axes values.
        glm::vec2 m_left_stick { 0.f, 0.f };
        glm::vec2 m_right_stick { 0.f, 0.f };
        float m_left_trigger { 0.f };
        float m_right_trigger { 0.f };

        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // vn