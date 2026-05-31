#include "vinter/input/input_map.hpp"

#include <string_view>
#include <vector>

#include "vinter/input/device_manager.hpp"

namespace vn
{
    InputMap::InputMap(DeviceManager& devices) : m_devices(devices)
    {
    }

    void InputMap::bind(const std::string_view action_name, Keyboard::Key key)
    {
        m_bindings[to_action_id(action_name)].push_back({.input_method = key});
    }

    void InputMap::bind(const std::string_view action_name, Mouse::Button button)
    {
        m_bindings[to_action_id(action_name)].push_back({.input_method = button});
    }

    void InputMap::bind(const std::string_view action_name, Mouse::Wheel wheel)
    {
        m_bindings[to_action_id(action_name)].push_back({.input_method = wheel});
    }

    void InputMap::bind(const std::string_view action_name, Gamepad::Button button)
    {
        m_bindings[to_action_id(action_name)].push_back({.input_method = button});
    }

    void InputMap::bind(const std::string_view action_name, Gamepad::Axis axis)
    {
        m_bindings[to_action_id(action_name)].push_back({.input_method = axis});
    }

    void
    InputMap::bind(const std::string_view action_name, Gamepad::Button button, std::size_t slot)
    {
        m_bindings[to_action_id(action_name)].push_back(
            {.input_method = button, .gamepad_slot = slot}
        );
    }

    void InputMap::bind(const std::string_view action_name, Gamepad::Axis axis, std::size_t slot)
    {
        m_bindings[to_action_id(action_name)].push_back(
            {.input_method = axis, .gamepad_slot = slot}
        );
    }

    auto InputMap::is_action_pressed(const std::string_view action_name) const -> bool
    {
        return check_action_pressed_state(action_name, PressedState::Pressed);
    }

    auto InputMap::is_action_just_pressed(const std::string_view action_name) const -> bool
    {
        return check_action_pressed_state(action_name, PressedState::JustPressed);
    }

    auto InputMap::is_action_just_released(const std::string_view action_name) const -> bool
    {
        return check_action_pressed_state(action_name, PressedState::JustReleased);
    }

    auto InputMap::get_action_strength(const std::string_view action_name) const -> float
    {
        const auto it = m_bindings.find(to_action_id(action_name));
        if (it == m_bindings.end())
        {
            return 0.f;
        }

        float max_strength = 0.f;
        for (const Binding& binding : it->second)
        {
            max_strength = std::max(max_strength, evaluate_input_strength(binding));
        }
        return max_strength;
    }

    auto InputMap::check_action_pressed_state(
        const std::string_view action_name,
        const PressedState state
    ) const -> bool
    {
        const auto it = m_bindings.find(to_action_id(action_name));
        if (it == m_bindings.end())
        {
            return false;
        }

        for (const Binding& binding : it->second)
        {
            if (evaluate_binding_pressed(binding, state))
            {
                return true;
            }
        }
        return false;
    }

    auto InputMap::evaluate_binding_pressed(const Binding& binding, const PressedState state) const
        -> bool
    {
        return std::visit(
            [&]<typename T>(T input_visitor) -> bool {
                using InputT = std::decay_t<T>;

                if constexpr (std::is_same_v<InputT, Keyboard::Key>)
                {
                    return evaluate_key_pressed_state(input_visitor, state);
                }
                else if constexpr (std::is_same_v<InputT, Mouse::Button>)
                {
                    return evaluate_mouse_button_pressed_state(input_visitor, state);
                }
                else if constexpr (std::is_same_v<InputT, Mouse::Wheel>)
                {
                    return evaluate_mouse_wheel_pressed_state(input_visitor, state);
                }
                else if constexpr (std::is_same_v<InputT, Gamepad::Button>)
                {
                    return evaluate_gamepad_button_pressed_state(input_visitor, binding, state);
                }
                else if constexpr (std::is_same_v<InputT, Gamepad::Axis>)
                {
                    return evaluate_gamepad_axis_pressed_state(input_visitor, binding, state);
                }
            },
            binding.input_method
        );
    }

    auto InputMap::evaluate_input_strength(const Binding& binding) const -> float
    {
        return std::visit(
            [&]<typename T>(T input_val) -> float {
                using InputT = std::decay_t<T>;

                if constexpr (std::is_same_v<InputT, Keyboard::Key>)
                {
                    return m_devices.get_keyboard().is_key_pressed(input_val) ? 1.f : 0.f;
                }
                else if constexpr (std::is_same_v<InputT, Mouse::Button>)
                {
                    return m_devices.get_mouse().is_button_pressed(input_val) ? 1.f : 0.f;
                }
                else if constexpr (std::is_same_v<InputT, Mouse::Wheel>)
                {
                    return m_devices.get_mouse().is_wheel_triggered(input_val) ? 1.f : 0.f;
                }
                else if constexpr (std::is_same_v<InputT, Gamepad::Button>)
                {
                    if (binding.gamepad_slot)
                    {
                        auto* g = m_devices.get_gamepad(*binding.gamepad_slot);
                        if (!g)
                        {
                            return 0.f; // SAFE
                        }
                        return g->is_button_pressed(input_val) ? 1.f : 0.f;
                    }
                    float max_strength = 0.f;
                    for (auto* g : m_devices.get_gamepads())
                    {
                        if (!g)
                        {
                            continue;
                        }
                        max_strength = std::max(
                            max_strength, g->is_button_pressed(input_val) ? 1.f : 0.f
                        );
                    }
                    return max_strength;
                }
                else if constexpr (std::is_same_v<InputT, Gamepad::Axis>)
                {
                    if (binding.gamepad_slot)
                    {
                        auto* g = m_devices.get_gamepad(*binding.gamepad_slot);
                        if (!g)
                        {
                            return 0.f; // SAFE
                        }
                        return g->get_axis_strength(input_val);
                    }
                    float max_strength = 0.f;
                    for (auto* g : m_devices.get_gamepads())
                    {
                        if (!g)
                        {
                            continue;
                        }
                        max_strength = std::max(max_strength, g->get_axis_strength(input_val));
                    }
                    return max_strength;
                }
            },
            binding.input_method
        );
    }

    auto
    InputMap::evaluate_key_pressed_state(const Keyboard::Key key, const PressedState state) const
        -> bool
    {
        switch (state)
        {
            case PressedState::Pressed: return m_devices.get_keyboard().is_key_pressed(key);
            case PressedState::JustPressed:
                return m_devices.get_keyboard().is_key_just_pressed(key);
            case PressedState::JustReleased:
                return m_devices.get_keyboard().is_key_just_released(key);
        }
        return false;
    }

    auto InputMap::evaluate_mouse_button_pressed_state(
        const Mouse::Button button,
        const PressedState state
    ) const -> bool
    {
        switch (state)
        {
            case PressedState::Pressed: return m_devices.get_mouse().is_button_pressed(button);
            case PressedState::JustPressed:
                return m_devices.get_mouse().is_button_just_pressed(button);
            case PressedState::JustReleased:
                return m_devices.get_mouse().is_button_just_released(button);
        }
        return false;
    }

    auto InputMap::evaluate_mouse_wheel_pressed_state(
        const Mouse::Wheel wheel,
        const PressedState state
    ) const -> bool
    {
        switch (state)
        {
            case PressedState::JustPressed: return m_devices.get_mouse().is_wheel_triggered(wheel);
            case PressedState::Pressed:
            case PressedState::JustReleased: return false;
        }
        return false;
    }

    auto InputMap::evaluate_gamepad_button_pressed_state(
        const Gamepad::Button button,
        const Binding& binding,
        const PressedState state
    ) const -> bool
    {
        // Check slot specific pressed state.
        if (binding.gamepad_slot)
        {
            const auto* gamepad = m_devices.get_gamepad(*binding.gamepad_slot);
            if (gamepad == nullptr)
            {
                return false;
            }
            switch (state)
            {
                case PressedState::Pressed: return gamepad->is_button_pressed(button);
                case PressedState::JustPressed: return gamepad->is_button_just_pressed(button);
                case PressedState::JustReleased: return gamepad->is_button_just_released(button);
            }
            // Check all gamepads for pressed state.
        }
        else
        {
            for (const auto* gamepad : m_devices.get_gamepads())
            {
                if (gamepad == nullptr)
                {
                    continue;
                }
                switch (state)
                {
                    case PressedState::Pressed:
                        if (gamepad->is_button_pressed(button))
                        {
                            return true;
                        }
                    case PressedState::JustPressed:
                        if (gamepad->is_button_just_pressed(button))
                        {
                            return true;
                        }
                    case PressedState::JustReleased:
                        if (gamepad->is_button_just_released(button))
                        {
                            return true;
                        }
                }
            }
        }
        return false;
    }

    auto InputMap::evaluate_gamepad_axis_pressed_state(
        const Gamepad::Axis axis,
        const Binding& binding,
        const PressedState state
    ) const -> bool
    {
        // Check slot specific pressed state.
        if (binding.gamepad_slot)
        {
            const auto* gamepad = m_devices.get_gamepad(*binding.gamepad_slot);
            if (gamepad == nullptr)
            {
                return false;
            }
            switch (state)
            {
                case PressedState::Pressed: return gamepad->is_axis_pressed(axis);
                case PressedState::JustPressed: return gamepad->is_axis_just_pressed(axis);
                case PressedState::JustReleased: return gamepad->is_axis_just_released(axis);
            }
            // Check all gamepads for pressed state.
        }
        else
        {
            for (const auto* gamepad : m_devices.get_gamepads())
            {
                if (gamepad == nullptr)
                {
                    continue;
                }
                switch (state)
                {
                    case PressedState::Pressed:
                        if (gamepad->is_axis_pressed(axis))
                        {
                            return true;
                        }
                    case PressedState::JustPressed:
                        if (gamepad->is_axis_just_pressed(axis))
                        {
                            return true;
                        }
                    case PressedState::JustReleased:
                        if (gamepad->is_axis_just_released(axis))
                        {
                            return true;
                        }
                }
            }
        }
        return false;
    }
} // namespace vn