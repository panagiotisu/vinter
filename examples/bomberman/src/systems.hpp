#pragma once

#include <cmath>
#include <queue>
#include <vector>

#include <vinter/app.hpp>

#include "components.hpp"

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