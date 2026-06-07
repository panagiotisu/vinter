#pragma once

#include <memory>
#include <ranges>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace vn {
    class GameObject;

    class Component {
        friend class GameObject;

    public:
        Component() = default;
        virtual ~Component() = default;

        [[nodiscard]]
        constexpr auto get_owner() const noexcept -> GameObject* {
            return m_owner;
        }

    protected:
        template <typename T>
        requires std::derived_from<T, Component>
        [[nodiscard]]
        auto get_component() -> T*;

        template <typename T>
        requires std::derived_from<T, Component>
        [[nodiscard]]
        auto get_component() -> const T*;

        template <typename T>
        [[nodiscard]]
        auto has_component() const -> bool;

        template <typename T>
        void remove_component() const;

        virtual void on_register() {
        }

        virtual void ready() {
        }

        virtual void update(float delta) {
        }

        virtual void on_remove() {
        }

    private:
        GameObject* m_owner {};
    };

    class GameObject {
    public:
        GameObject() = default;

        void update(float delta) {
            for (const auto& component : m_components | std::views::values) {
                component->update(delta);
            }
        }

        auto add_child(GameObject child) -> GameObject& {
            child.m_parent = this;

            auto ptr = std::make_unique<GameObject>(std::move(child));
            GameObject& ref = *ptr;

            m_children.emplace_back(std::move(ptr));
            return ref;
        }

        template <typename T>
        auto add_component(T&& component) -> T& {
            assert_is_base_of_component<T>();
            auto component_ptr = std::make_unique<T>(std::forward<T>(component));
            return add_component_impl(std::move(component_ptr));
        }

        template <typename T, typename... Args>
        auto add_component(Args&&... args) -> T& {
            assert_is_base_of_component<T>();
            auto component_ptr = std::make_unique<T>(std::forward<Args>(args)...);
            return add_component_impl(std::move(component_ptr));
        }

        template <typename T>
        [[nodiscard]]
        auto has_component() const -> bool {
            assert_is_base_of_component<T>();
            return m_components.contains(std::type_index(typeid(T)));
        }

        template <typename T>
        auto get_component() -> T* {
            assert_is_base_of_component<T>();

            const auto it = m_components.find(std::type_index(typeid(T)));
            return it != m_components.end() ? static_cast<T*>(it->second.get()) : nullptr;
        }

        template <typename T>
        auto get_component() const -> const T* {
            assert_is_base_of_component<T>();

            const auto it = m_components.find(std::type_index(typeid(T)));
            return it != m_components.end() ? static_cast<const T*>(it->second.get()) : nullptr;
        }

        template <typename T>
        void remove_component() {
            assert_is_base_of_component<T>();

            const auto it = m_components.find(std::type_index(typeid(T)));
            if (it == m_components.end()) {
                return;
            }

            it->second->on_remove();
            it->second->m_owner = nullptr;
            m_components.erase(it);
        }

    private:
        std::unordered_map<std::type_index, std::unique_ptr<Component>> m_components;
        std::vector<std::unique_ptr<GameObject>> m_children;
        GameObject* m_parent {nullptr};

        template <typename T>
        void assert_is_base_of_component() const {
            static_assert(std::is_base_of_v<Component, T>, "T must derive from Component.");
        }

        template <typename T>
        auto add_component_impl(std::unique_ptr<T> component_ptr) -> T& {
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

    template <typename T>
    requires std::derived_from<T, Component>
    auto Component::get_component() -> T* {
        return m_owner ? m_owner->get_component<T>() : nullptr;
    }

    template <typename T>
    requires std::derived_from<T, Component>
    auto Component::get_component() -> const T* {
        return m_owner ? m_owner->get_component<T>() : nullptr;
    }

    template <typename T>
    auto Component::has_component() const -> bool {
        return m_owner && m_owner->has_component<T>();
    }

    template <typename T>
    void Component::remove_component() const {
        if (m_owner) {
            m_owner->remove_component<T>();
        }
    }
} // namespace vn