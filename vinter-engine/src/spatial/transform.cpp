#include "vinter/spatial/transform.hpp"

#include <queue>

namespace vn::system {
    void resolve_transform_tree(ECS& ecs) {
        // Find all root entities; entities whose transform.parent == vn::Entity::Null.
        std::vector<vn::Entity> roots {};
        ecs.view<component::Transform>().for_each([&](vn::Entity entity,
                                                      component::Transform& transform) {
            if (transform.parent.is_null()) {
                roots.push_back(entity);
            }
        });

        // Breadth-first traversal queue.
        std::queue<vn::Entity> queue {};
        for (auto root : roots) {
            queue.push(root);
        }

        while (!queue.empty()) {
            vn::Entity entity = queue.front();
            queue.pop();

            auto& transform = ecs.get<component::Transform>(entity);

            // Compute global transform.
            if (transform.parent.is_null()) {
                // No parent.
                transform.global = transform.local;
            } else {
                const auto& parent_transform = ecs.get<component::Transform>(transform.parent);
                transform.global = parent_transform.global * transform.local;
            }

            // Enqueue children.
            ecs.view<component::Transform>().for_each([&](vn::Entity child,
                                                          const component::Transform& transform) {
                if (transform.parent == entity) {
                    queue.push(child);
                }
            });
        }
    }
} // namespace vn::system