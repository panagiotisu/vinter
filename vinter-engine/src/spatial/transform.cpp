#include "vinter/spatial/transform.hpp"

#include <queue>

#include "vinter/ecs/database.hpp"

namespace vn::ecs {
    ResolveTransformTreeSystem::ResolveTransformTreeSystem(Database& database)
        : ISystem(database) {};

    void ResolveTransformTreeSystem::update(float /*delta*/) {
        // Find all root entities; entities whose transform.parent == vn::Entity::Null.
        std::vector<Entity> roots {};
        m_database.query<Transform>().for_each([&](Entity entity, Transform& transform) {
            if (transform.parent.is_null()) {
                roots.push_back(entity);
            }
        });

        // Breadth-first traversal queue.
        std::queue<vn::ecs::Entity> queue {};
        for (auto root : roots) {
            queue.push(root);
        }

        while (!queue.empty()) {
            Entity entity = queue.front();
            queue.pop();

            auto& transform = m_database.get<Transform>(entity);

            // Compute global transform.
            if (transform.parent.is_null()) {
                // No parent.
                transform.global = transform.local;
            } else {
                const auto& parent_transform = m_database.get<Transform>(transform.parent);
                transform.global = parent_transform.global * transform.local;
            }

            // Enqueue children.
            m_database.query<Transform>().for_each([&](Entity child, const Transform& transform) {
                if (transform.parent == entity) {
                    queue.push(child);
                }
            });
        }
    }
} // namespace vn::ecs
