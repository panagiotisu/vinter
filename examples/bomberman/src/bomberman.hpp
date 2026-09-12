#pragma once

#include <vinter/app.hpp>

#include "systems.hpp"

class Bomberman : public vn::App {
public:
    explicit Bomberman(const vn::ProjectSettings& project_settings) : App(project_settings) {
    }

protected:
    void load() override {
        m_font = get_fonts().load("../../../examples/bomberman/assets/fonts/ponde___.ttf", 24);

        get_input().bind("move_right", vn::Keyboard::Key::D);
        get_input().bind("move_left", vn::Keyboard::Key::A);
        get_input().bind("move_up", vn::Keyboard::Key::W);
        get_input().bind("move_down", vn::Keyboard::Key::S);

        m_player = get_ecs().create_entity("Player");
        get_ecs().add<vn::ecs::Transform>(
            m_player,
            { 
                .local = { 
                    .position = {
                        get_window().get_width() / 2,
                        get_window().get_height() / 2,
                    } 
                },
                .global = { 
                    .position = {
                        get_window().get_width() / 2,
                        get_window().get_height() / 2,
                    } 
                } 
            }
        );
        get_ecs().add<LinearKinematics>(
            m_player,
            {
                .max_speed = 300.f,
                .acceleration_coefficient = 9.f,
            }
        );
        get_ecs().add<Player>(m_player);

        m_player_visuals = get_ecs().create_entity();
        get_ecs().add<vn::ecs::Transform>(m_player_visuals, { .parent = m_player });
        auto& player_sprite = get_ecs().add<vn::ecs::Sprite>(
            m_player_visuals,
            vn::ecs::Sprite::create(
                get_textures().load("../../../examples/bomberman/assets/textures/player.png"), 36, 1
            )
        );

        auto& player_sprite_animator = get_ecs().add<vn::ecs::SpriteAnimator>(m_player_visuals);
        player_sprite_animator.add("idle_down", 0, 3, 3, true);
        player_sprite_animator.add("idle_downright", 4, 7, 3, true);
        player_sprite_animator.add("idle_upright", 8, 11, 3, true);
        player_sprite_animator.add("idle_up", 12, 15, 3, true);
        player_sprite_animator.add("walking_down", 16, 19, 6, true);
        player_sprite_animator.add("walking_downright", 20, 23, 8, true);
        player_sprite_animator.add("walking_upright", 24, 27, 8, true);
        player_sprite_animator.add("walking_up", 28, 31, 6, true);
        player_sprite_animator.play(player_sprite, "idle_down");

        get_ecs().add<SpriteDirector>(
            m_player_visuals, { .face_pattern = SpriteDirector::FacePattern::Hexagon }
        );

        get_systems().add(vn::ecs::ResolveTransformTreeSystem(get_ecs()));
        get_systems().add(PlayerInputSystem(get_ecs(), get_input(), get_devices().get_mouse()));
        get_systems().add(DirectionalAnimationSystem(get_ecs()));
        get_systems().add(KinematicsIntegrationSystem(get_ecs()));
        get_systems().add(vn::ecs::SpriteSystem(get_ecs(), get_renderer()));
    }

    void update(float /*delta*/) override {
        auto& player_sprite_transform = get_ecs().get<vn::ecs::Transform>(m_player_visuals);
        if (get_devices().get_mouse().is_wheel_triggered(vn::Mouse::Wheel::Up)) {
            player_sprite_transform.local.scale *= 1.5f;
        } else if (get_devices().get_mouse().is_wheel_triggered(vn::Mouse::Wheel::Down)) {
            player_sprite_transform.local.scale /= 1.5f;
        }

        get_window().set_title(std::format("Bomberman | FPS: {}", get_time().get_filtered_fps()));
    }

    void render() override {
    }

private:
    vn::ecs::Entity m_player {};
    vn::ecs::Entity m_player_visuals {};
    vn::Font m_font {};
};
