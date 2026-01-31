#pragma once

#include <unordered_map>
#include <vector>
#include <string_view>
#include <cstdint>

#include "vinter/input/keyboard.hpp"
#include "vinter/input/mouse.hpp"
#include "vinter/utils/hash.hpp"

namespace vn {
    using ActionID = std::uint64_t;

    struct InputBinding {
        std::function<bool()> is_pressed;
        std::function<bool()> is_just_pressed;
        std::function<bool()> is_just_released;
    };

    class InputMap {
    public:
        InputMap(Keyboard& keyboard, Mouse& mouse)
            : m_keyboard(keyboard), m_mouse(mouse) {}

        [[nodiscard]] static constexpr ActionID action_id(std::string_view action_name) noexcept {
            return fnv1a_64(action_name);
        }

        template<typename T>
        void bind(std::string_view action_name, T button) {
            InputBinding binding;

            if constexpr (std::is_same_v<T, Key>) {
                binding.is_pressed = [this, button](){ return m_keyboard.is_key_pressed(button); };
                binding.is_just_pressed = [this, button](){ return m_keyboard.is_key_just_pressed(button); };
                binding.is_just_released = [this, button](){ return m_keyboard.is_key_just_released(button); };
            }
            else if constexpr (std::is_same_v<T, MouseButton>) {
                binding.is_pressed = [this, button](){ return m_mouse.is_button_pressed(button); };
                binding.is_just_pressed = [this, button](){ return m_mouse.is_button_just_pressed(button); };
                binding.is_just_released = [this, button](){ return m_mouse.is_button_just_released(button); };
            }
            else {
                static_assert(always_false<T>, "Unsupported input type");
            }

            m_bindings[action_id(action_name)].push_back(std::move(binding));
        }

        [[nodiscard]] bool is_action_pressed(std::string_view action_name) const {
            return check_action(action_name, &InputBinding::is_pressed);
        }

        [[nodiscard]] bool is_action_just_pressed(std::string_view action_name) const {
            return check_action(action_name, &InputBinding::is_just_pressed);
        }

        [[nodiscard]] bool is_action_just_released(std::string_view action_name) const {
            return check_action(action_name, &InputBinding::is_just_released);
        }

    private:
        template<typename Fn>
        bool check_action(std::string_view action_name, Fn fn) const {
            const auto it = m_bindings.find(action_id(action_name));
            if (it == m_bindings.end()) return false;
            for (const auto& binding : it->second) {
                if ((binding.*fn)()) return true;
            }
            return false;
        }

        Keyboard& m_keyboard;
        Mouse& m_mouse;
        std::unordered_map<uint64_t, std::vector<InputBinding>> m_bindings;

        template<class T> static constexpr bool always_false = false;
    };
} // vn
