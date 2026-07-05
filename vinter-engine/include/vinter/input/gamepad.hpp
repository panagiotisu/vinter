#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "vinter/color.hpp"
#include "vinter/input/button_states.hpp"

union SDL_Event;
struct SDL_Gamepad;

namespace vn {
    struct Color;

    class Gamepad {
        friend class DeviceManager;

    public:
        explicit Gamepad(unsigned int joystick_id);
        ~Gamepad();

        enum class Button : std::uint8_t {
            // Common Buttons.
            South,
            East,
            West,
            North,
            Back,
            Guide,
            Start,
            LeftStick,
            RightStick,
            LeftShoulder,
            RightShoulder,
            DpadUp,
            DpadDown,
            DpadLeft,
            DpadRight,

            // Specialized Buttons that depend on gamepad company and model.
            RightPaddle1,
            LeftPaddle1,
            RightPaddle2,
            LeftPaddle2,
            Touchpad,
            Misc1, /* Additional button (e.g. Xbox Series X share button, PS5 microphone button,
                      Nintendo Switch Pro capture button, Amazon Luna microphone button, Google
                      Stadia capture button). */
            Misc2,
            Misc3, // Additional button (e.g. Nintendo GameCube left trigger click).
            Misc4, // Additional button (e.g. Nintendo GameCube right trigger click).
            Misc5,
            Misc6,
        };

        enum class Axis : std::uint8_t {
            LeftStickLeft,
            LeftStickRight,
            LeftStickUp,
            LeftStickDown,
            RightStickLeft,
            RightStickRight,
            RightStickUp,
            RightStickDown,
            LeftTrigger,
            RightTrigger,
            Count,
        };

        enum class ButtonLabel : std::uint8_t {
            Unknown,
            A,
            B,
            X,
            Y,
            Cross,
            Circle,
            Square,
            Triangle,
        };

        enum class Type : std::uint8_t {
            Unknown,
            Standard,
            Xbox360,
            XboxOne,
            Ps3,
            Ps4,
            Ps5,
            Switch,
            JoyconLeft,
            JoyconRight,
            JoyconPair,
            GameCube,
        };

        [[nodiscard]]
        auto get_id() const noexcept -> unsigned int;

        [[nodiscard]]
        auto get_guid_string() const noexcept -> std::string;

        [[nodiscard]]
        auto get_name() const noexcept -> std::string;

        [[nodiscard]]
        auto get_type() const noexcept -> Type;

        [[nodiscard]]
        auto get_button_label(Button button) const noexcept -> ButtonLabel;

        [[nodiscard]]
        auto is_button_pressed(Button button) const noexcept -> bool;

        [[nodiscard]]
        auto is_button_just_pressed(Button button) const noexcept -> bool;

        [[nodiscard]]
        auto is_button_just_released(Button button) const noexcept -> bool;

        [[nodiscard]]
        auto is_axis_pressed(Axis axis) const noexcept -> bool;

        [[nodiscard]]
        auto is_axis_just_pressed(Axis axis) const noexcept -> bool;

        [[nodiscard]]
        auto is_axis_just_released(Axis axis) const noexcept -> bool;

        [[nodiscard]]
        auto get_axis_strength(Axis axis) const noexcept -> float;

        void begin_vibrate(
            float weak_percent_magnitude,
            float strong_percent_magnitude,
            float duration_sec = 0.f
        ) const;
        void stop_vibrate() const;

        void set_led_color(Color color) const;

    private:
        void handle_events(const SDL_Event& event);
        void update();

        [[nodiscard]]
        static auto normalize_axis(float axis) noexcept -> float;
        static void apply_trigger_deadzone(float& trigger_value, float deadzone);
        static void apply_stick_deadzone(float& stick_x, float& stick_y, float deadzone);
        [[nodiscard]]
        static auto axis_to_index(Gamepad::Axis axis) -> std::size_t;

        [[nodiscard]]
        static auto to_sdl_gamepad_button(Button button) noexcept -> int;
        void remap_sdl_axes_to_gamepad_axes();

    private:
        float m_stick_deadzone { 0.1f };
        float m_trigger_deadzone { 0.05f };

        SDL_Gamepad* m_sdl_gamepad {};
        ButtonStates m_button_states;
        std::vector<float> m_sdl_axis_states_current {};
        std::vector<float> m_sdl_axis_states_previous {};
        std::array<float, static_cast<std::size_t>(Axis::Count)> m_axis_states_current {};
        std::array<float, static_cast<std::size_t>(Axis::Count)> m_axis_states_previous {};
    };
} // namespace vn