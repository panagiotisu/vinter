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
        enum class Type {
            Key,
            MouseButton
        };

        Type type;

        union {
            Key key;
            MouseButton mouse_button;
        };

        static InputBinding from_key(const Key k) {
            InputBinding b {};
            b.type = Type::Key;
            b.key = k;
            return b;
        }

        static InputBinding from_mouse(const MouseButton mb) {
            InputBinding b {};
            b.type = Type::MouseButton;
            b.mouse_button = mb;
            return b;
        }
    };

    class InputMap {
    public:
        InputMap(Keyboard& keyboard, Mouse& mouse)
            : m_keyboard(keyboard), m_mouse(mouse) {}

        [[nodiscard]] static constexpr ActionID action_id(std::string_view name) noexcept {
            return fnv1a_64(name);
        }

        void bind(std::string_view action_name, const Key key) {
            m_bindings[action_id(action_name)].push_back(
                InputBinding::from_key(key)
            );
        }

        void bind(std::string_view action_name, const MouseButton button) {
            m_bindings[action_id(action_name)].push_back(
                InputBinding::from_mouse(button)
            );
        }

        [[nodiscard]] bool is_action_pressed(std::string_view action_name) const {
            return check(action_name, Query::Pressed);
        }

        [[nodiscard]] bool is_action_just_pressed(std::string_view action_name) const {
            return check(action_name, Query::JustPressed);
        }

        [[nodiscard]] bool is_action_just_released(std::string_view action_name) const {
            return check(action_name, Query::JustReleased);
        }

    private:
        enum class Query {
            Pressed,
            JustPressed,
            JustReleased
        };

        bool check(std::string_view action_name, const Query query) const {
            const auto it = m_bindings.find(action_id(action_name));
            if (it == m_bindings.end())
                return false;

            for (const InputBinding& binding : it->second) {
                if (evaluate(binding, query))
                    return true;
            }

            return false;
        }

        bool evaluate(const InputBinding& b, const Query q) const {
            switch (b.type) {
                case InputBinding::Type::Key:
                    return eval_key(b.key, q);

                case InputBinding::Type::MouseButton:
                    return eval_mouse(b.mouse_button, q);
            }
            return false;
        }

        bool eval_key(const Key key, const Query q) const {
            switch (q) {
                case Query::Pressed:
                    return m_keyboard.is_key_pressed(key);
                case Query::JustPressed:
                    return m_keyboard.is_key_just_pressed(key);
                case Query::JustReleased:
                    return m_keyboard.is_key_just_released(key);
            }
            return false;
        }

        bool eval_mouse(const MouseButton button, const Query q) const {
            switch (q) {
                case Query::Pressed:
                    return m_mouse.is_button_pressed(button);
                case Query::JustPressed:
                    return m_mouse.is_button_just_pressed(button);
                case Query::JustReleased:
                    return m_mouse.is_button_just_released(button);
            }
            return false;
        }

        Keyboard& m_keyboard;
        Mouse& m_mouse;
        std::unordered_map<ActionID, std::vector<InputBinding>> m_bindings;
    };

} // vn
