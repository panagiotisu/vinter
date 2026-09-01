#pragma once

#include <cmath>

#include <vinter/app.hpp>

#include "components.hpp"

class PlayerInputSystem : public vn::ecs::ISystem {
public:
    PlayerInputSystem(vn::ecs::Database& database, vn::InputMap& input)
        : ISystem(database)
        , m_input(input) {
    }

    void update(float /*delta*/) override {
        m_database.query<Player, LinearKinematics>().for_each([&](const Player&,
                                                                  LinearKinematics& lkin) {
            lkin.acceleration_direction = {};
            if (m_input.is_action_pressed("move_right")) {
                lkin.acceleration_direction.x += 1.f;
            }
            if (m_input.is_action_pressed("move_left")) {
                lkin.acceleration_direction.x -= 1.f;
            }
            if (m_input.is_action_pressed("move_up")) {
                lkin.acceleration_direction.y -= 1.f;
            }
            if (m_input.is_action_pressed("move_down")) {
                lkin.acceleration_direction.y += 1.f;
            }
            if (lkin.acceleration_direction != glm::vec2 {}) {
                lkin.acceleration_direction = glm::normalize(lkin.acceleration_direction);
            }
        });
    }

private:
    vn::InputMap& m_input;
};

class KinematicsIntegrationSystem : public vn::ecs::ISystem {
public:
    explicit KinematicsIntegrationSystem(vn::ecs::Database& database) : ISystem(database) {
    }

    void update(float delta) override {
        integrate_accelerations(delta);
        integrate_velocities(delta);
    }

private:
    void integrate_accelerations(float delta) {
        m_database.query<LinearKinematics>().for_each([&](LinearKinematics& lkin) {
            const glm::vec2 target_velocity = lkin.acceleration_direction * lkin.max_speed;
            const float speed_delta = lkin.acceleration_coefficient * delta;

            lkin.velocity.x = std::lerp(lkin.velocity.x, target_velocity.x, speed_delta);
            lkin.velocity.y = std::lerp(lkin.velocity.y, target_velocity.y, speed_delta);
        });
    }

    void integrate_velocities(float delta) {
        m_database.query<vn::ecs::Transform, LinearKinematics>().for_each(
            [&](vn::ecs::Transform& t, const LinearKinematics& lkin) {
                t.local.position += lkin.velocity * delta;
            }
        );
    }
};
