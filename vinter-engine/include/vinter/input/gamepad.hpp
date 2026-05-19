#pragma once

#include <cstdint>
#include <memory>
#include <string>

union SDL_Event;

namespace vn {
    struct Color;

    class Gamepad {
        friend class DeviceManager;

    public:
        explicit Gamepad(unsigned int joystickId);
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
        auto GetId() const noexcept -> unsigned int;

        [[nodiscard]]
        auto GetGuidString() const noexcept -> std::string;

        [[nodiscard]]
        auto GetName() const noexcept -> std::string;

        [[nodiscard]]
        auto GetType() const noexcept -> Type;

        [[nodiscard]]
        auto GetButtonLabel(Button button) const noexcept -> ButtonLabel;

        [[nodiscard]]
        auto IsButtonPressed(Button button) const noexcept -> bool;

        [[nodiscard]]
        auto IsButtonJustPressed(Button button) const noexcept -> bool;

        [[nodiscard]]
        auto IsButtonJustReleased(Button button) const noexcept -> bool;

        [[nodiscard]]
        auto IsAxisPressed(Axis axis) const noexcept -> bool;

        [[nodiscard]]
        auto IsAxisJustPressed(Axis axis) const noexcept -> bool;

        [[nodiscard]]
        auto IsAxisJustReleased(Axis axis) const noexcept -> bool;

        [[nodiscard]]
        auto GetAxisStrength(Axis axis) const noexcept -> float;

        void BeginVibrate(
            float weakPercentMagnitude,
            float strongPercentMagnitude,
            float durationSec = 0.f
        ) const;
        void StopVibrate() const;

        void SetLedColor(Color color) const;

    private:
        void HandleEvents(const SDL_Event& event);
        void Update();

        float m_stickDeadzone {0.1f};
        float m_triggerDeadzone {0.05f};

        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // namespace vn