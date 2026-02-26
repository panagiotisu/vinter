#pragma once

#include <unordered_map>
#include <vector>
#include <string_view>
#include <cstdint>
#include <variant>
#include <optional>
#include <algorithm>

#include "vinter/input/keyboard.hpp"
#include "vinter/input/mouse.hpp"
#include "vinter/input/gamepad.hpp"
#include "vinter/utils/hash.hpp"

namespace vn {
    using ActionID = std::uint64_t;

    using Input = std::variant<
        Keyboard::Key,
        Mouse::Button, Mouse::Wheel,
        Gamepad::Button, Gamepad::Axis
    >;

    // Represents a binding of an input to an action, optionally for a specific gamepad slot.
    struct Binding {
        Input input;
        std::optional<std::size_t> gamepad_slot; // nullopt = all gamepads
    };

    class InputMap {
    public:
        explicit InputMap(DeviceManager& devices)
            : m_devices(devices) {}

        [[nodiscard]] static constexpr ActionID to_action_id(std::string_view name) noexcept {
            return fnv1a_64(name);
        }

        // Keyboard / Mouse bindings.
        void bind(std::string_view action_name, Keyboard::Key key) {
            m_bindings[to_action_id(action_name)].push_back({key, std::nullopt});
        }
        void bind(std::string_view action_name, Mouse::Button button) {
            m_bindings[to_action_id(action_name)].push_back({button, std::nullopt});
        }
        void bind(std::string_view action_name, Mouse::Wheel wheel) {
            m_bindings[to_action_id(action_name)].push_back({wheel, std::nullopt});
        }

        // Gamepad bindings (all gamepads).
        void bind(std::string_view action_name, Gamepad::Button button) {
            m_bindings[to_action_id(action_name)].push_back({button, std::nullopt});
        }
        void bind(std::string_view action_name, Gamepad::Axis axis) {
            m_bindings[to_action_id(action_name)].push_back({axis, std::nullopt});
        }

        // Gamepad bindings (specific slot).
        void bind(std::string_view action_name, Gamepad::Button button, std::size_t slot) {
            m_bindings[to_action_id(action_name)].push_back({button, slot});
        }
        void bind(std::string_view action_name, Gamepad::Axis axis, std::size_t slot) {
            m_bindings[to_action_id(action_name)].push_back({axis, slot});
        }

        // Pressed state queries.
        [[nodiscard]] bool is_action_pressed(std::string_view action_name) const {
            return check_action_pressed_state(action_name, PressedState::Pressed);
        }
        [[nodiscard]] bool is_action_just_pressed(std::string_view action_name) const {
            return check_action_pressed_state(action_name, PressedState::JustPressed);
        }
        [[nodiscard]] bool is_action_just_released(std::string_view action_name) const {
            return check_action_pressed_state(action_name, PressedState::JustReleased);
        }

        // Returns strength of action mapped to gamepad axes in the range [0.0 - 1.0], and either 0 or 1 for discrete
        // input methods like keys, buttons and mouse wheels.
        [[nodiscard]] float get_action_strength(std::string_view action_name) const {
            const auto it = m_bindings.find(to_action_id(action_name));
            if (it == m_bindings.end()) return 0.f;

            float max_strength = 0.f;
            for (const Binding& binding : it->second) {
                max_strength = std::max(max_strength, evaluate_input_strength(binding));
            }
            return max_strength;
        }

    private:
        enum class PressedState {
            Pressed,
            JustPressed,
            JustReleased
        };

        bool check_action_pressed_state(std::string_view action_name, PressedState state) const {
            const auto it = m_bindings.find(to_action_id(action_name));
            if (it == m_bindings.end()) return false;

            for (const Binding& binding : it->second) {
                if (evaluate_binding_pressed(binding, state))
                    return true;
            }
            return false;
        }

        bool evaluate_binding_pressed(const Binding& binding, PressedState state) const {
            return std::visit([&]<typename T>(T input_val) -> bool {
                using InputT = std::decay_t<T>;

                if constexpr (std::is_same_v<InputT, Keyboard::Key>) {
                    return evaluate_key_pressed_state(input_val, state);
                } else if constexpr (std::is_same_v<InputT, Mouse::Button>) {
                    return evaluate_mouse_button_pressed_state(input_val, state);
                } else if constexpr (std::is_same_v<InputT, Mouse::Wheel>) {
                    return evaluate_mouse_wheel_pressed_state(input_val, state);
                } else if constexpr (std::is_same_v<InputT, Gamepad::Button>) {
                    if (binding.gamepad_slot) {
                        auto* g = m_devices.get_gamepad(*binding.gamepad_slot);
                        if (!g) return false; // SAFE: null slot
                        switch (state) {
                            case PressedState::Pressed: return g->is_button_pressed(input_val);
                            case PressedState::JustPressed: return g->is_button_just_pressed(input_val);
                            case PressedState::JustReleased: return g->is_button_just_released(input_val);
                        }
                    } else {
                        for (auto* g : m_devices.get_gamepads()) {
                            if (!g) continue; // skip null
                            switch (state) {
                                case PressedState::Pressed: if (g->is_button_pressed(input_val)) return true;
                                case PressedState::JustPressed: if (g->is_button_just_pressed(input_val)) return true;
                                case PressedState::JustReleased: if (g->is_button_just_released(input_val)) return true;
                            }
                        }
                        return false;
                    }
                } else if constexpr (std::is_same_v<InputT, Gamepad::Axis>) {
                    if (binding.gamepad_slot) {
                        auto* g = m_devices.get_gamepad(*binding.gamepad_slot);
                        if (!g) return false; // SAFE: null slot
                        switch (state) {
                            case PressedState::Pressed: return g->is_axis_pressed(input_val);
                            case PressedState::JustPressed: return g->is_axis_just_pressed(input_val);
                            case PressedState::JustReleased: return g->is_axis_just_released(input_val);
                        }
                    } else {
                        for (auto* g : m_devices.get_gamepads()) {
                            if (!g) continue; // skip null
                            switch (state) {
                                case PressedState::Pressed: if (g->is_axis_pressed(input_val)) return true;
                                case PressedState::JustPressed: if (g->is_axis_just_pressed(input_val)) return true;
                                case PressedState::JustReleased: if (g->is_axis_just_released(input_val)) return true;
                            }
                        }
                        return false;
                    }
                }
            }, binding.input);
        }

        float evaluate_input_strength(const Binding& binding) const {
            return std::visit([&]<typename T>(T input_val) -> float {
                using InputT = std::decay_t<T>;

                if constexpr (std::is_same_v<InputT, Keyboard::Key>) {
                    return m_devices.get_keyboard().is_key_pressed(input_val) ? 1.f : 0.f;
                } else if constexpr (std::is_same_v<InputT, Mouse::Button>) {
                    return m_devices.get_mouse().is_button_pressed(input_val) ? 1.f : 0.f;
                } else if constexpr (std::is_same_v<InputT, Mouse::Wheel>) {
                    return m_devices.get_mouse().is_wheel_triggered(input_val) ? 1.f : 0.f;
                } else if constexpr (std::is_same_v<InputT, Gamepad::Button>) {
                    if (binding.gamepad_slot) {
                        auto* g = m_devices.get_gamepad(*binding.gamepad_slot);
                        if (!g) return 0.f; // SAFE
                        return g->is_button_pressed(input_val) ? 1.f : 0.f;
                    }
                    float max_strength = 0.f;
                    for (auto* g : m_devices.get_gamepads()) {
                        if (!g) continue;
                        max_strength = std::max(max_strength, g->is_button_pressed(input_val) ? 1.f : 0.f);
                    }
                    return max_strength;
                } else if constexpr (std::is_same_v<InputT, Gamepad::Axis>) {
                    if (binding.gamepad_slot) {
                        auto* g = m_devices.get_gamepad(*binding.gamepad_slot);
                        if (!g) return 0.f; // SAFE
                        return g->get_axis_strength(input_val);
                    }
                    float max_strength = 0.f;
                    for (auto* g : m_devices.get_gamepads()) {
                        if (!g) continue;
                        max_strength = std::max(max_strength, g->get_axis_strength(input_val));
                    }
                    return max_strength;
                }
            }, binding.input);
        }

        bool evaluate_key_pressed_state(const Keyboard::Key key, const PressedState state) const {
            switch (state) {
                case PressedState::Pressed: return m_devices.get_keyboard().is_key_pressed(key);
                case PressedState::JustPressed: return m_devices.get_keyboard().is_key_just_pressed(key);
                case PressedState::JustReleased: return m_devices.get_keyboard().is_key_just_released(key);
            }
            return false;
        }

        bool evaluate_mouse_button_pressed_state(const Mouse::Button button, const PressedState state) const {
            switch (state) {
                case PressedState::Pressed: return m_devices.get_mouse().is_button_pressed(button);
                case PressedState::JustPressed: return m_devices.get_mouse().is_button_just_pressed(button);
                case PressedState::JustReleased: return m_devices.get_mouse().is_button_just_released(button);
            }
            return false;
        }

        bool evaluate_mouse_wheel_pressed_state(const Mouse::Wheel wheel, const PressedState state) const {
            switch (state) {
                case PressedState::JustPressed: return m_devices.get_mouse().is_wheel_triggered(wheel);
                case PressedState::Pressed:
                case PressedState::JustReleased:
                    return false;
            }
            return false;
        }

        DeviceManager& m_devices;
        std::unordered_map<ActionID, std::vector<Binding>> m_bindings;
    };
} // vn