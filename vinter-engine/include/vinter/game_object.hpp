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
        constexpr auto GetOwner() const noexcept -> GameObject* {
            return m_owner;
        }

    protected:
        template <typename T>
        requires std::derived_from<T, Component> [[nodiscard]]
        auto GetComponent() -> T*;

        template <typename T>
        requires std::derived_from<T, Component> [[nodiscard]]
        auto GetComponent() -> const T*;

        template <typename T>
        [[nodiscard]]
        auto HasComponent() const -> bool;

        template <typename T>
        void RemoveComponent() const;

        virtual void OnRegister() {
        }

        virtual void Ready() {
        }

        virtual void Update(float delta) {
        }

        virtual void OnRemove() {
        }

    private:
        GameObject* m_owner {};
    };

    class GameObject {
    public:
        GameObject() = default;

        void Update(float delta) {
            for (const auto& component : m_components | std::views::values) {
                component->Update(delta);
            }
        }

        void AddChild(GameObject& gameObject) {
            gameObject.m_parent = this;
            m_children.emplace_back(gameObject);
        }

        template <typename T>
        auto AddComponent(T&& component) -> T& {
            AssertIsBaseOfComponent<T>();
            auto componentPtr = std::make_unique<T>(std::forward<T>(component));
            return AddComponentImpl(std::move(componentPtr));
        }

        template <typename T, typename... Args>
        auto AddComponent(Args&&... args) -> T& {
            AssertIsBaseOfComponent<T>();
            auto componentPtr = std::make_unique<T>(std::forward<Args>(args)...);
            return AddComponentImpl(std::move(componentPtr));
        }

        template <typename T>
        [[nodiscard]]
        auto HasComponent() const -> bool {
            AssertIsBaseOfComponent<T>();
            return m_components.contains(std::type_index(typeid(T)));
        }

        template <typename T>
        auto GetComponent() -> T* {
            AssertIsBaseOfComponent<T>();

            const auto it = m_components.find(std::type_index(typeid(T)));
            return it != m_components.end() ? static_cast<T*>(it->second.get()) : nullptr;
        }

        template <typename T>
        auto GetComponent() const -> const T* {
            AssertIsBaseOfComponent<T>();

            const auto it = m_components.find(std::type_index(typeid(T)));
            return it != m_components.end() ? static_cast<const T*>(it->second.get()) : nullptr;
        }

        template <typename T>
        void RemoveComponent() {
            AssertIsBaseOfComponent<T>();

            const auto it = m_components.find(std::type_index(typeid(T)));
            if (it == m_components.end()) {
                return;
            }

            it->second->OnRemove();
            it->second->m_owner = nullptr;
            m_components.erase(it);
        }

    private:
        std::unordered_map<std::type_index, std::unique_ptr<Component>> m_components;
        std::vector<GameObject> m_children;
        GameObject* m_parent {nullptr};

        template <typename T>
        void AssertIsBaseOfComponent() const {
            static_assert(std::is_base_of_v<Component, T>, "T must derive from Component.");
        }

        template <typename T>
        auto AddComponentImpl(std::unique_ptr<T> componentPtr) -> T& {
            auto key = std::type_index(typeid(T));

            auto [it, inserted] = m_components.try_emplace(key, std::move(componentPtr));
            Component* base = it->second.get();

            if (inserted) {
                base->m_owner = this;
                base->OnRegister();
            }

            return *static_cast<T*>(base);
        }
    };

    template <typename T> // clang-format off
    requires std::derived_from<T, Component>
    auto Component::GetComponent() -> T* { // clang-format on
        return m_owner ? m_owner->GetComponent<T>() : nullptr;
    }

    template <typename T> // clang-format off
    requires std::derived_from<T, Component>
    auto Component::GetComponent() -> const T* { // clang-format on
        return m_owner ? m_owner->GetComponent<T>() : nullptr;
    }

    template <typename T>
    auto Component::HasComponent() const -> bool {
        return m_owner && m_owner->HasComponent<T>();
    }

    template <typename T>
    void Component::RemoveComponent() const {
        if (m_owner) {
            m_owner->RemoveComponent<T>();
        }
    }
} // namespace vn