#pragma once

#include <optional>
#include <unordered_map>
#include <variant>

#include "vinter/input/gamepad.hpp"
#include "vinter/input/keyboard.hpp"
#include "vinter/input/mouse.hpp"
#include "vinter/utils/hash.hpp"

namespace vn {
    class Devices;

    /**
     * @brief The unique hashed identifier corresponding to an action name.
     */
    using ActionID = std::uint64_t;

    /**
     * @brief The type of device-specific input method to bind to a generic input action.
     */
    using InputMethod =
        std::variant<Keyboard::Key, Mouse::Button, Mouse::Wheel, Gamepad::Button, Gamepad::Axis>;

    /**
     * @brief Represents a binding of an input method to an action, optionally for a specific
     * gamepad slot.
     *
     * @note If the gamepad slot is nullopt, then it corresponds to either a non-gamepad device,
     * or all gamepad slots simultaneously.
     */
    struct Binding {
        InputMethod input_method;
        std::optional<std::size_t> gamepad_slot;
    };

    /**
     * @brief Maps named actions to input devices and queries their state.
     *
     * InputMap allows the registration of logical actions (like "jump", "shoot", "punch")
     * and their binding to one or more physical inputs, such as keyboard keys, mouse buttons,
     * mouse wheel directions, or gamepad buttons and axes.
     *
     * The class provides:
     * - Continuous pressed state queries (`is_action_pressed`)
     * - Single-frame events (`is_action_just_pressed`, `is_action_just_released`)
     * - Action strength queries (`get_action_strength`) for analog inputs like gamepad axes.
     *
     * Each binding can be device-agnostic (applies to all connected gamepads) or tied
     * to a specific device slot. InputMap queries all active devices safely, even if
     * some gamepads are disconnected.
     *
     * Typical usage:
     * @code{.cpp}
     * // Binding actions to physical input device methods.
     * get_input.bind("jump", Keyboard::Key::T);              // Binds to keyboard's 'T' key.
     * get_input.bind("jump", Mouse::Button::Middle);         // Binds to mouse's 'Middle' button.
     * get_input.bind("jump", Mouse::Wheel::Up);              // Binds to mousewheel's 'Up' scroll.
     * get_input.bind("jump", Gamepad::Button::East);         // Binds to all gamepads' 'East'
     * button. get_input->bind("jump", Gamepad::Axis::LeftStickUp, 0); // Binds to 'Left Stick Up
     * Axis' of gamepad at slot '0'.
     *
     * // Querying registered actions.
     * if (get_input->is_action_just_pressed("jump")) {
     *      player.jump();
     * }
     *
     * player.jump_velocity.y = get_input->get_action_strength("jump");
     * @endcode
     *
     * @note InputMap requires a valid DeviceManager reference for querying device states.
     */
    class InputMap {
    public:
        /**
         * @brief Constructs an InputMap object after taking in a reference to a DeviceManager
         * object.
         *
         * @param devices The reference to the DeviceManager object.
         */
        explicit InputMap(Devices& devices);

        /**
         * @brief Binds a registered action to a key.
         *
         * @param action_name The name of a registered action.
         * @param key The key to be bound.
         */
        void bind(std::string_view action_name, Keyboard::Key key);

        /**
         * @brief Binds a registered action to a mouse button.
         *
         * @param action_name The name of a registered action.
         * @param button The mouse button to be bound.
         */
        void bind(std::string_view action_name, Mouse::Button button);

        /**
         * @brief Binds a registered action to a mouse wheel input.
         *
         * @param action_name The name of a registered action.
         * @param wheel The mouse wheel input to be bound.
         */
        void bind(std::string_view action_name, Mouse::Wheel wheel);

        /**
         * @brief Binds a registered action to a gamepad button, for all gamepads.
         *
         * @param action_name The name of a registered action.
         * @param button The gamepad button to be bound.
         */
        void bind(std::string_view action_name, Gamepad::Button button);

        /**
         * @brief Binds a registered action to a gamepad axis, for all gamepads.
         *
         * @param action_name The name of a registered action.
         * @param axis The gamepad axis to be bound.
         */
        void bind(std::string_view action_name, Gamepad::Axis axis);

        /**
         * @brief Binds a registered action to a gamepad button, for a specified gamepad.
         *
         * @param action_name The name of a registered action.
         * @param button The gamepad button to be bound.
         * @param slot The slot number of the gamepad to be bound.
         */
        void bind(std::string_view action_name, Gamepad::Button button, std::size_t slot);

        /**
         * @brief Binds a registered action to a gamepad axis, for a specified gamepad.
         *
         * @param action_name The name of a registered action.
         * @param axis The gamepad axis to be bound.
         * @param slot The slot number of the gamepad to be bound.
         */
        void bind(std::string_view action_name, Gamepad::Axis axis, std::size_t slot);

        /**
         * @brief Checks if a registered action is actively pressed during the current frame.
         *
         * @param action_name The name of a registered action.
         * @return `true` if the action is currently pressed, `false` otherwise.
         */
        [[nodiscard]]
        bool is_action_pressed(std::string_view action_name) const;

        /**
         * @brief Checks if a registered action was pressed this frame but not in the previous
         * frame.
         *
         * @note This is useful for detecting a single press event rather than continuous holding.
         *
         * @param action_name The name of a registered action.
         * @return `true` if the action was just pressed in the current frame, `false` otherwise.
         */
        [[nodiscard]]
        bool is_action_just_pressed(std::string_view action_name) const;

        /**
         * @brief Checks if a registered action was released this frame but was pressed in the
         * previous frame.
         *
         * @note This is useful for detecting a single release event.
         *
         * @param action_name The name of a registered action.
         * @return `true` if the action was just released in the current frame, `false` otherwise.
         */
        [[nodiscard]]
        bool is_action_just_released(std::string_view action_name) const;

        /**
         * @brief Returns the normalized strength of the specified action in the range [0.0, 1.0].
         *
         * Actions mapped to analog inputs (e.g., gamepad axes) produce continuous values within
         * [0.0, 1.0], while actions mapped to digital inputs (e.g., buttons) return 1.0 when
         * pressed and 0.0 otherwise.
         *
         * If the action is not registered, this function returns 0.0.
         *
         * @param action_name The name of the action.
         * @return The normalized strength of the action in the range [0.0, 1.0].
         */
        [[nodiscard]]
        float get_action_strength(std::string_view action_name) const;

    private:
        enum class PressedState : std::uint8_t { Pressed, JustPressed, JustReleased };

        [[nodiscard]]
        static constexpr ActionID to_action_id(const std::string_view name) noexcept {
            return fnv1a64(name);
        }

        [[nodiscard]]
        bool check_action_pressed_state(std::string_view action_name, PressedState state) const;

        [[nodiscard]]
        bool evaluate_binding_pressed(const Binding& binding, PressedState state) const;

        [[nodiscard]]
        float evaluate_input_strength(const Binding& binding) const;

        [[nodiscard]]
        bool evaluate_key_pressed_state(Keyboard::Key key, PressedState state) const;

        [[nodiscard]]
        bool evaluate_mouse_button_pressed_state(Mouse::Button button, PressedState state) const;

        [[nodiscard]]
        bool evaluate_mouse_wheel_pressed_state(Mouse::Wheel wheel, PressedState state) const;

        [[nodiscard]]
        bool evaluate_gamepad_button_pressed_state(
            Gamepad::Button button,
            const Binding& binding,
            PressedState state
        ) const;

        [[nodiscard]]
        bool evaluate_gamepad_axis_pressed_state(
            Gamepad::Axis axis,
            const Binding& binding,
            PressedState state
        ) const;

        Devices& m_devices;
        std::unordered_map<ActionID, std::vector<Binding>> m_bindings {};
    };
} // namespace vn