#pragma once

#include <memory>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "vinter/ecs/database.hpp"

namespace vn::ecs {
    class ISystem {
    public:
        explicit ISystem(Database& database) : m_database(database) {
        }

        virtual ~ISystem() = default;

        virtual void pre_update() {
        }

        virtual void update(float delta) {
        }

        virtual void render() {
        }

        virtual void post_update() {
        }

    protected:
        Database& m_database;
    };

    class SystemQueue {
    private:
        struct SystemEntry {
            std::unique_ptr<ISystem> system;
            std::type_index type_idx;
        };

    public:
        template <typename T>
        requires std::derived_from<std::decay_t<T>, ISystem>
        void add(T&& system) {
            using ConcreteType = std::decay_t<T>;
            auto type_idx = std::type_index(typeid(ConcreteType));

            if (m_type_indices.contains(type_idx)) {
                remove<ConcreteType>();
            }

            size_t index = m_systems.size();
            m_type_indices[type_idx] = index;
            m_systems.emplace_back(
                std::make_unique<ConcreteType>(std::forward<T>(system)), type_idx
            );

            VN_INFO("Registered ECS system: {}", demangle(typeid(ConcreteType).name()));
        }

        template <typename T>
        requires std::derived_from<std::decay_t<T>, ISystem>
        void remove() {
            using ConcreteType = std::decay_t<T>;
            auto type_idx = std::type_index(typeid(ConcreteType));

            auto it = m_type_indices.find(type_idx);
            if (it == m_type_indices.end()) {
                return; // Not found
            }

            size_t index_to_remove = it->second;
            size_t last_index = m_systems.size() - 1;

            // If we're not removing the last element, move the last element into the hole.
            if (index_to_remove != last_index) {
                // Move the entire entry (unique_ptr + type_index).
                m_systems[index_to_remove] = std::move(m_systems[last_index]);

                // Update the map entry for the moved system to its new index.
                // We use the stored type_idx instead of calling typeid()!
                m_type_indices[m_systems[index_to_remove].type_idx] = index_to_remove;
            }

            m_systems.pop_back();
            m_type_indices.erase(it);

            VN_INFO("Removed ECS system: {}", demangle(typeid(ConcreteType).name()));
        }

        template <typename T>
        requires std::derived_from<std::decay_t<T>, ISystem>
        [[nodiscard]]
        T* try_get() {
            using ConcreteType = std::decay_t<T>;
            auto type_idx = std::type_index(typeid(ConcreteType));

            auto it = m_type_indices.find(type_idx);
            if (it != m_type_indices.end()) {
                return static_cast<ConcreteType*>(m_systems[it->second].system.get());
            }
            return nullptr;
        }

        template <typename T>
        requires std::derived_from<std::decay_t<T>, ISystem>
        [[nodiscard]]
        T& get() const {
            auto* sys = try_get<T>();
            VN_ASSERT(
                sys != nullptr,
                "System of type {} does not exist in the queue.",
                demangle(typeid(T).name())
            );
            return *sys;
        }

        template <typename T>
        requires std::derived_from<std::decay_t<T>, ISystem>
        [[nodiscard]]
        const T& get() const {
            const auto* sys = try_get<T>();
            VN_ASSERT(
                sys != nullptr,
                "System of type {} does not exist in the queue.",
                demangle(typeid(T).name())
            );
            return *sys;
        }

        void pre_update() {
            for (auto& entry : m_systems) {
                entry.system->pre_update();
            }
        }

        void update(float delta) {
            for (auto& entry : m_systems) {
                entry.system->update(delta);
            }
        }

        void render() {
            for (auto& entry : m_systems) {
                entry.system->render();
            }
        }

        void post_update() {
            for (auto& entry : m_systems) {
                entry.system->post_update();
            }
        }

    private:
        std::vector<SystemEntry> m_systems;
        std::unordered_map<std::type_index, size_t> m_type_indices;
    };
} // namespace vn::ecs