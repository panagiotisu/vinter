#pragma once

#include <cmath>
#include <queue>
#include <vector>

#include <vinter/app.hpp>

#include "components.hpp"

inline Transform2D::Data
combine_transforms(const Transform2D::Data& parent, const Transform2D::Data& local) {
    Transform2D::Data result {};

    float sin = std::sin(parent.rotation);
    float cos = std::cos(parent.rotation);

    glm::vec2 scaled = local.position * parent.scale;
    glm::vec2 rotated = {
        scaled.x * cos - scaled.y * sin,
        scaled.x * sin + scaled.y * cos,
    };

    result.position = parent.position + rotated;
    result.scale = parent.scale * local.scale;
    result.rotation = parent.rotation + local.rotation;

    return result;
}

inline void transform_hierarchy_system(vn::ECS& ecs) {
    // Find all root entities; entities whose transform.parent == vn::Entity::Null.
    std::vector<vn::Entity> roots {};
    ecs.view<Transform2D>().for_each([&](vn::Entity entity, Transform2D& transform) {
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

        auto& transform = ecs.get<Transform2D>(entity);

        // Compute global transform.
        if (transform.parent.is_null()) {
            // No parent.
            transform.global = transform.local;
        } else {
            const auto& parent_transform = ecs.get<Transform2D>(transform.parent);
            transform.global = combine_transforms(parent_transform.global, transform.local);
        }

        // Enqueue children.
        ecs.view<Transform2D>().for_each([&](vn::Entity child, const Transform2D& transform) {
            if (transform.parent == entity) {
                queue.push(child);
            }
        });
    }
}

inline void player_input_system(vn::ECS& ecs, vn::InputMap& input) {
    auto view = ecs.view<Player, LinearKinematics2D>();
    view.for_each([&](const Player&, LinearKinematics2D& lkin) {
        lkin.acceleration_direction = {};
        if (input.is_action_pressed("move_right")) {
            lkin.acceleration_direction.x += 1.f;
        }
        if (input.is_action_pressed("move_left")) {
            lkin.acceleration_direction.x -= 1.f;
        }
        if (input.is_action_pressed("move_up")) {
            lkin.acceleration_direction.y -= 1.f;
        }
        if (input.is_action_pressed("move_down")) {
            lkin.acceleration_direction.y += 1.f;
        }
        if (lkin.acceleration_direction != glm::vec2 {}) {
            lkin.acceleration_direction = glm::normalize(lkin.acceleration_direction);
        }
    });
}

inline void integrate_acceleration_system(vn::ECS& ecs, float delta) {
    auto view = ecs.view<LinearKinematics2D>();
    view.for_each([&](LinearKinematics2D& lkin) {
        const glm::vec2 target_velocity = lkin.acceleration_direction * lkin.max_speed;
        const float speed_delta = lkin.acceleration_coefficient * delta;

        lkin.velocity.x = std::lerp(lkin.velocity.x, target_velocity.x, speed_delta);
        lkin.velocity.y = std::lerp(lkin.velocity.y, target_velocity.y, speed_delta);
    });
}

inline void integrate_velocity_system(vn::ECS& ecs, float delta) {
    auto view = ecs.view<Transform2D, LinearKinematics2D>();
    view.for_each([&](Transform2D& t, const LinearKinematics2D& lkin) {
        t.local.position += lkin.velocity * delta;
    });
}

inline void circle_drawing_system(vn::ECS& ecs, vn::Renderer& renderer) {
    auto view = ecs.view<Circle, Transform2D>();
    view.for_each([&](const Circle& circle, const Transform2D& transform) {
        renderer.draw_circle(transform.global.position, circle.radius, circle.color);
    });
}

inline void aabb_drawing_system(vn::ECS& ecs, vn::Renderer& renderer) {
    auto view = ecs.view<AABB, Transform2D>();
    view.for_each([&](const AABB& aabb, const Transform2D& transform) {
        renderer.draw_aabb(transform.global.position, aabb.size, aabb.color);
    });
}