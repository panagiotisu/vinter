#include "vinter/input/input_map.hpp"

#include <string_view>
#include <vector>

#include "vinter/input/device_manager.hpp"

namespace vn {
    InputMap::InputMap(DeviceManager& devices) : m_devices(devices) {
    }

    void InputMap::Bind(const std::string_view actionName, Keyboard::Key key) {
        m_bindings[ToActionId(actionName)].push_back({.input_method = key});
    }

    void InputMap::Bind(const std::string_view actionName, Mouse::Button button) {
        m_bindings[ToActionId(actionName)].push_back({.input_method = button});
    }

    void InputMap::Bind(const std::string_view actionName, Mouse::Wheel wheel) {
        m_bindings[ToActionId(actionName)].push_back({.input_method = wheel});
    }

    void InputMap::Bind(const std::string_view actionName, Gamepad::Button button) {
        m_bindings[ToActionId(actionName)].push_back({.input_method = button});
    }

    void InputMap::Bind(const std::string_view actionName, Gamepad::Axis axis) {
        m_bindings[ToActionId(actionName)].push_back({.input_method = axis});
    }

    void
    InputMap::Bind(const std::string_view actionName, Gamepad::Button button, std::size_t slot) {
        m_bindings[ToActionId(actionName)].push_back({.input_method = button, .gamepad_slot = slot}
        );
    }

    void InputMap::Bind(const std::string_view actionName, Gamepad::Axis axis, std::size_t slot) {
        m_bindings[ToActionId(actionName)].push_back({.input_method = axis, .gamepad_slot = slot});
    }

    auto InputMap::IsActionPressed(const std::string_view actionName) const -> bool {
        return CheckActionPressedState(actionName, PressedState::Pressed);
    }

    auto InputMap::IsActionJustPressed(const std::string_view actionName) const -> bool {
        return CheckActionPressedState(actionName, PressedState::JustPressed);
    }

    auto InputMap::IsActionJustReleased(const std::string_view actionName) const -> bool {
        return CheckActionPressedState(actionName, PressedState::JustReleased);
    }

    auto InputMap::GetActionStrength(const std::string_view actionName) const -> float {
        const auto it = m_bindings.find(ToActionId(actionName));
        if (it == m_bindings.end()) {
            return 0.f;
        }

        float maxStrength = 0.f;
        for (const Binding& binding : it->second) {
            maxStrength = std::max(maxStrength, EvaluateInputStrength(binding));
        }
        return maxStrength;
    }

    auto InputMap::CheckActionPressedState(
        const std::string_view actionName,
        const PressedState state
    ) const -> bool {
        const auto it = m_bindings.find(ToActionId(actionName));
        if (it == m_bindings.end()) {
            return false;
        }

        for (const Binding& binding : it->second) {
            if (EvaluateBindingPressed(binding, state)) {
                return true;
            }
        }
        return false;
    }

    auto InputMap::EvaluateBindingPressed(const Binding& binding, const PressedState state) const
        -> bool {
        return std::visit(
            [&]<typename T>(T inputVisitor) -> bool {
                using InputT = std::decay_t<T>;

                if constexpr (std::is_same_v<InputT, Keyboard::Key>) {
                    return EvaluateKeyPressedState(inputVisitor, state);
                } else if constexpr (std::is_same_v<InputT, Mouse::Button>) {
                    return EvaluateMouseButtonPressedState(inputVisitor, state);
                } else if constexpr (std::is_same_v<InputT, Mouse::Wheel>) {
                    return EvaluateMouseWheelPressedState(inputVisitor, state);
                } else if constexpr (std::is_same_v<InputT, Gamepad::Button>) {
                    return EvaluateGamepadButtonPressedState(inputVisitor, binding, state);
                } else if constexpr (std::is_same_v<InputT, Gamepad::Axis>) {
                    return EvaluateGamepadAxisPressedState(inputVisitor, binding, state);
                }
            },
            binding.input_method
        );
    }

    auto InputMap::EvaluateInputStrength(const Binding& binding) const -> float {
        return std::visit(
            [&]<typename T>(T inputVal) -> float {
                using InputT = std::decay_t<T>;

                if constexpr (std::is_same_v<InputT, Keyboard::Key>) {
                    return m_devices.GetKeyboard().IsKeyPressed(inputVal) ? 1.f : 0.f;
                } else if constexpr (std::is_same_v<InputT, Mouse::Button>) {
                    return m_devices.GetMouse().IsButtonPressed(inputVal) ? 1.f : 0.f;
                } else if constexpr (std::is_same_v<InputT, Mouse::Wheel>) {
                    return m_devices.GetMouse().IsWheelTriggered(inputVal) ? 1.f : 0.f;
                } else if constexpr (std::is_same_v<InputT, Gamepad::Button>) {
                    if (binding.gamepad_slot) {
                        auto* g = m_devices.GetGamepad(*binding.gamepad_slot);
                        if (!g) {
                            return 0.f; // SAFE
                        }
                        return g->IsButtonPressed(inputVal) ? 1.f : 0.f;
                    }
                    float maxStrength = 0.f;
                    for (auto* g : m_devices.GetGamepads()) {
                        if (!g) {
                            continue;
                        }
                        maxStrength =
                            std::max(maxStrength, g->IsButtonPressed(inputVal) ? 1.f : 0.f);
                    }
                    return maxStrength;
                } else if constexpr (std::is_same_v<InputT, Gamepad::Axis>) {
                    if (binding.gamepad_slot) {
                        auto* g = m_devices.GetGamepad(*binding.gamepad_slot);
                        if (!g) {
                            return 0.f; // SAFE
                        }
                        return g->GetAxisStrength(inputVal);
                    }
                    float maxStrength = 0.f;
                    for (auto* g : m_devices.GetGamepads()) {
                        if (!g) {
                            continue;
                        }
                        maxStrength = std::max(maxStrength, g->GetAxisStrength(inputVal));
                    }
                    return maxStrength;
                }
            },
            binding.input_method
        );
    }

    auto InputMap::EvaluateKeyPressedState(const Keyboard::Key key, const PressedState state) const
        -> bool {
        switch (state) {
            case PressedState::Pressed: return m_devices.GetKeyboard().IsKeyPressed(key);
            case PressedState::JustPressed: return m_devices.GetKeyboard().IsKeyJustPressed(key);
            case PressedState::JustReleased: return m_devices.GetKeyboard().IsKeyJustReleased(key);
        }
        return false;
    }

    auto InputMap::EvaluateMouseButtonPressedState(
        const Mouse::Button button,
        const PressedState state
    ) const -> bool {
        switch (state) {
            case PressedState::Pressed: return m_devices.GetMouse().IsButtonPressed(button);
            case PressedState::JustPressed: return m_devices.GetMouse().IsButtonJustPressed(button);
            case PressedState::JustReleased:
                return m_devices.GetMouse().IsButtonJustReleased(button);
        }
        return false;
    }

    auto InputMap::EvaluateMouseWheelPressedState(
        const Mouse::Wheel wheel,
        const PressedState state
    ) const -> bool {
        switch (state) {
            case PressedState::JustPressed: return m_devices.GetMouse().IsWheelTriggered(wheel);
            case PressedState::Pressed:
            case PressedState::JustReleased: return false;
        }
        return false;
    }

    auto InputMap::EvaluateGamepadButtonPressedState(
        const Gamepad::Button button,
        const Binding& binding,
        const PressedState state
    ) const -> bool {
        // Check slot specific pressed state.
        if (binding.gamepad_slot) {
            const auto* gamepad = m_devices.GetGamepad(*binding.gamepad_slot);
            if (gamepad == nullptr) {
                return false;
            }
            switch (state) {
                case PressedState::Pressed: return gamepad->IsButtonPressed(button);
                case PressedState::JustPressed: return gamepad->IsButtonJustPressed(button);
                case PressedState::JustReleased: return gamepad->IsButtonJustReleased(button);
            }
            // Check all gamepads for pressed state.
        } else {
            for (const auto* gamepad : m_devices.GetGamepads()) {
                if (gamepad == nullptr) {
                    continue;
                }
                switch (state) {
                    case PressedState::Pressed:
                        if (gamepad->IsButtonPressed(button)) {
                            return true;
                        }
                    case PressedState::JustPressed:
                        if (gamepad->IsButtonJustPressed(button)) {
                            return true;
                        }
                    case PressedState::JustReleased:
                        if (gamepad->IsButtonJustReleased(button)) {
                            return true;
                        }
                }
            }
        }
        return false;
    }

    auto InputMap::EvaluateGamepadAxisPressedState(
        const Gamepad::Axis axis,
        const Binding& binding,
        const PressedState state
    ) const -> bool {
        // Check slot specific pressed state.
        if (binding.gamepad_slot) {
            const auto* gamepad = m_devices.GetGamepad(*binding.gamepad_slot);
            if (gamepad == nullptr) {
                return false;
            }
            switch (state) {
                case PressedState::Pressed: return gamepad->IsAxisPressed(axis);
                case PressedState::JustPressed: return gamepad->IsAxisJustPressed(axis);
                case PressedState::JustReleased: return gamepad->IsAxisJustReleased(axis);
            }
            // Check all gamepads for pressed state.
        } else {
            for (const auto* gamepad : m_devices.GetGamepads()) {
                if (gamepad == nullptr) {
                    continue;
                }
                switch (state) {
                    case PressedState::Pressed:
                        if (gamepad->IsAxisPressed(axis)) {
                            return true;
                        }
                    case PressedState::JustPressed:
                        if (gamepad->IsAxisJustPressed(axis)) {
                            return true;
                        }
                    case PressedState::JustReleased:
                        if (gamepad->IsAxisJustReleased(axis)) {
                            return true;
                        }
                }
            }
        }
        return false;
    }
} // namespace vn