#pragma once

#include <memory>
#include <ranges>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <type_traits>

#include "vinter/component.hpp"

namespace vn {
    class GameObject {
    public:
        // ======================================================================================================== //
        // GameObject API
        // ======================================================================================================== //
        void update(float delta) {
            for (const auto& component : m_components | std::views::values) {
                component->update(delta);
            }
        }

        void add_child(GameObject& game_object) {
            game_object.m_parent = this;
            m_children.emplace_back(game_object);
        }

        // ======================================================================================================== //
        // Component API
        // ======================================================================================================== //
        template<typename T>
        T& add_component(T&& component) {
            assert_is_base_of_component<T>();
            auto component_ptr = std::make_unique<T>(std::forward<T>(component));
            return add_component_impl(std::move(component_ptr));
        }

        template<typename T, typename... Args>
        T& add_component(Args&&... args) {
            assert_is_base_of_component<T>();
            auto component_ptr = std::make_unique<T>(std::forward<Args>(args)...);
            return add_component_impl(std::move(component_ptr));
        }

        template<typename T>
        [[nodiscard]] bool has_component() const {
            assert_is_base_of_component<T>();
            return m_components.contains(std::type_index(typeid(T)));
        }

        template<typename T>
        T* get_component() {
            assert_is_base_of_component<T>();

            const auto it = m_components.find(std::type_index(typeid(T)));
            return it != m_components.end()
                ? static_cast<T*>(it->second.get())
                : nullptr;
        }

        template<typename T>
        const T* get_component() const {
            assert_is_base_of_component<T>();

            const auto it = m_components.find(std::type_index(typeid(T)));
            return it != m_components.end()
                ? static_cast<const T*>(it->second.get())
                : nullptr;
        }

        template<typename T>
        void remove_component() {
            assert_is_base_of_component<T>();

            const auto it = m_components.find(std::type_index(typeid(T)));
            if (it == m_components.end())
                return;

            it->second->on_remove();
            it->second->m_owner = nullptr;
            m_components.erase(it);
        }

    private:
        std::unordered_map<std::type_index, std::unique_ptr<Component>> m_components {};
        std::vector<GameObject> m_children {};
        GameObject* m_parent { nullptr };

        template<typename T>
        void assert_is_base_of_component() const {
            static_assert(std::is_base_of_v<Component, T>, "T must derive from Component.");
        }

        template<typename T>
        T& add_component_impl(std::unique_ptr<T> component_ptr) {
            auto key = std::type_index(typeid(T));

            auto [it, inserted] = m_components.try_emplace(key, std::move(component_ptr));
            Component* base = it->second.get();

            if (inserted) {
                base->m_owner = this;
                base->on_register();
            }

            return *static_cast<T*>(base);
        }
    };
} // vn

#include "component.inl"