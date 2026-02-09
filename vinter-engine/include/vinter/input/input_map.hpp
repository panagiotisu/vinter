#pragma once

#include <unordered_map>
#include <vector>
#include <string_view>
#include <cstdint>
#include <variant>

#include "vinter/input/keyboard.hpp"
#include "vinter/input/mouse.hpp"
#include "vinter/input/gamepad.hpp"
#include "vinter/utils/hash.hpp"

namespace vn {
    using ActionID = std::uint64_t;
    using Input = std::variant<Key, MouseButton, GamepadButton>;

    class InputMap {
    public:
        InputMap(Keyboard& keyboard, Mouse& mouse, Gamepad& gamepad)
            : m_keyboard(keyboard), m_mouse(mouse), m_gamepad(gamepad) {
        }

        [[nodiscard]] static constexpr ActionID action_id(std::string_view name) noexcept {
            return fnv1a_64(name);
        }

        void bind(std::string_view action_name, const Key key) {
            m_bindings[action_id(action_name)].emplace_back(key);
        }
        void bind(std::string_view action_name, const MouseButton mouse_button) {
            m_bindings[action_id(action_name)].emplace_back(mouse_button);
        }
        void bind(std::string_view action_name, const GamepadButton gamepad_button) {
            m_bindings[action_id(action_name)].emplace_back(gamepad_button);
        }

        [[nodiscard]] bool is_action_pressed(std::string_view action_name) const {
            return check_action_state(action_name, ActionState::Pressed);
        }
        [[nodiscard]] bool is_action_just_pressed(std::string_view action_name) const {
            return check_action_state(action_name, ActionState::JustPressed);
        }
        [[nodiscard]] bool is_action_just_released(std::string_view action_name) const {
            return check_action_state(action_name, ActionState::JustReleased);
        }

    private:
        enum class ActionState {
            Pressed,
            JustPressed,
            JustReleased
        };

        bool check_action_state(std::string_view action_name, const ActionState state) const {
            const auto it = m_bindings.find(action_id(action_name));
            if (it == m_bindings.end())
                return false;

            for (const Input& input : it->second) {
                if (evaluate(input, state))
                    return true;
            }

            return false;
        }

        bool evaluate(const Input& input, const ActionState state) const {
            return std::visit([&](auto input_visitor) {
                using T = std::decay_t<decltype(input_visitor)>;

                if constexpr (std::is_same_v<T, Key>) {
                    return eval_key(input_visitor, state);
                } else if constexpr (std::is_same_v<T, MouseButton>) {
                    return eval_mouse(input_visitor, state);
                } else if constexpr (std::is_same_v<T, GamepadButton>) {
                    return eval_gamepad(input_visitor, state);
                }
            }, input);
        }

        bool eval_key(const Key key, const ActionState state) const {
            switch (state) {
                case ActionState::Pressed:
                    return m_keyboard.is_key_pressed(key);
                case ActionState::JustPressed:
                    return m_keyboard.is_key_just_pressed(key);
                case ActionState::JustReleased:
                    return m_keyboard.is_key_just_released(key);
            }
            return false;
        }

        bool eval_mouse(const MouseButton button, const ActionState state) const {
            switch (state) {
                case ActionState::Pressed:
                    return m_mouse.is_button_pressed(button);
                case ActionState::JustPressed:
                    return m_mouse.is_button_just_pressed(button);
                case ActionState::JustReleased:
                    return m_mouse.is_button_just_released(button);
            }
            return false;
        }

        bool eval_gamepad(const GamepadButton button, const ActionState state) const {
            switch (state) {
                case ActionState::Pressed:
                    return m_gamepad.is_button_pressed(button);
                case ActionState::JustPressed:
                    return m_gamepad.is_button_just_pressed(button);
                case ActionState::JustReleased:
                    return m_gamepad.is_button_just_released(button);
            }
            return false;
        }

        Keyboard& m_keyboard;
        Mouse& m_mouse;
        Gamepad& m_gamepad;
        std::unordered_map<ActionID, std::vector<Input>> m_bindings;
    };

} // vn
