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

class DirectionalAnimationSystem : public vn::ecs::ISystem {
public:
    explicit DirectionalAnimationSystem(vn::ecs::Database& database) : ISystem(database) {
    }

    void update(float /* delta */) override {
        update_animation_cardinal_direction();
        update_sprite_flipping();
    }

private:
    void update_animation_cardinal_direction() {
        m_database.query<SpriteDirector, vn::ecs::SpriteAnimator>().for_each(
            [&](SpriteDirector& director, vn::ecs::SpriteAnimator& animator) {

            }
        );
    }

    void update_sprite_flipping() {
        m_database.query<vn::ecs::Transform, SpriteDirector>().for_each(
            [&](vn::ecs::Transform& transform, const SpriteDirector& director) {
                transform.local.scale.x = std::copysign(
                    transform.local.scale.x, director.face_direction.x
                );
            }
        );
    }

    [[nodiscard]]
    static SpriteDirector::Cardinal
    cardinal_from_vector(SpriteDirector::FacePattern face_pattern, glm::vec2 face_direction) {
        const auto angle = vn::Angle::from_vector(face_direction);
        const float angle_degrees_wrapped_unsigned = angle.wrap_unsigned().as_degrees();

        switch (face_pattern) {
            case SpriteDirector::FacePattern::Linear: return SpriteDirector::Cardinal::Right;
            case SpriteDirector::FacePattern::Square:
                return cardinal_from_square_pattern(angle_degrees_wrapped_unsigned);
            case SpriteDirector::FacePattern::Rhombus:
                return cardinal_from_rhombus_pattern(angle_degrees_wrapped_unsigned);
            case SpriteDirector::FacePattern::Hexagon:
                return cardinal_from_hexagon_pattern(angle_degrees_wrapped_unsigned);
            case SpriteDirector::FacePattern::Octagon:
                return cardinal_from_octagon_pattern(angle_degrees_wrapped_unsigned);
        }
    }

    [[nodiscard]]
    static SpriteDirector::Cardinal cardinal_from_square_pattern(float angle_degrees) {
        if (angle_degrees >= 0.f && angle_degrees < 180.f) {
            return SpriteDirector::Cardinal::DownRight;
        }
        if (angle_degrees >= 180.f && angle_degrees < 360.f) {
            return SpriteDirector::Cardinal::UpRight;
        }
        return SpriteDirector::Cardinal::DownRight;
    }

    [[nodiscard]]
    static SpriteDirector::Cardinal cardinal_from_rhombus_pattern(float angle_degrees) {
        if ((angle_degrees >= 315.f) || (angle_degrees < 45.f)) {
            return SpriteDirector::Cardinal::Right;
        }
        if (angle_degrees >= 45.f && angle_degrees < 135.f) {
            return SpriteDirector::Cardinal::Down;
        }
        if (angle_degrees >= 135.f && angle_degrees < 225.f) {
            return SpriteDirector::Cardinal::Right;
        }
        if (angle_degrees >= 225.f && angle_degrees < 315.f) {
            return SpriteDirector::Cardinal::Up;
        }
        return SpriteDirector::Cardinal::Right;
    }

    [[nodiscard]]
    static SpriteDirector::Cardinal cardinal_from_hexagon_pattern(float angle_degrees) {
        if (angle_degrees >= 0.f && angle_degrees < 60.f) {
            return SpriteDirector::Cardinal::DownRight;
        }
        if (angle_degrees >= 60.f && angle_degrees < 120.f) {
            return SpriteDirector::Cardinal::Down;
        }
        if (angle_degrees >= 120.f && angle_degrees < 180.f) {
            return SpriteDirector::Cardinal::DownRight;
        }
        if (angle_degrees >= 180.f && angle_degrees < 240.f) {
            return SpriteDirector::Cardinal::UpRight;
        }
        if (angle_degrees >= 240.f && angle_degrees < 300.f) {
            return SpriteDirector::Cardinal::Up;
        }
        if (angle_degrees >= 300.f && angle_degrees < 360.f) {
            return SpriteDirector::Cardinal::UpRight;
        }
        return SpriteDirector::Cardinal::DownRight;
    }

    [[nodiscard]]
    static SpriteDirector::Cardinal cardinal_from_octagon_pattern(float angle_degrees) {
        if ((angle_degrees >= 337.5f) || (angle_degrees < 22.5f)) {
            return SpriteDirector::Cardinal::Right;
        }
        if (angle_degrees >= 22.5f && angle_degrees < 67.5f) {
            return SpriteDirector::Cardinal::DownRight;
        }
        if (angle_degrees >= 67.5f && angle_degrees < 112.5f) {
            return SpriteDirector::Cardinal::Down;
        }
        if (angle_degrees >= 112.5f && angle_degrees < 157.5f) {
            return SpriteDirector::Cardinal::DownRight;
        }
        if (angle_degrees >= 157.5f && angle_degrees < 202.5f) {
            return SpriteDirector::Cardinal::Right;
        }
        if (angle_degrees >= 202.5f && angle_degrees < 247.5f) {
            return SpriteDirector::Cardinal::UpRight;
        }
        if (angle_degrees >= 247.5f && angle_degrees < 292.5f) {
            return SpriteDirector::Cardinal::Up;
        }
        if (angle_degrees >= 292.5f && angle_degrees < 337.5f) {
            return SpriteDirector::Cardinal::UpRight;
        }
        return SpriteDirector::Cardinal::Right;
    }
};