#pragma once

#include <vinter/app.hpp>

#include "systems.hpp"

class Bomberman : public vn::App {
public:
    explicit Bomberman(const vn::ProjectSettings& project_settings) : App(project_settings) {
    }

protected:
    void load() override {
        get_input().bind("move_right", vn::Keyboard::Key::D);
        get_input().bind("move_left", vn::Keyboard::Key::A);
        get_input().bind("move_up", vn::Keyboard::Key::W);
        get_input().bind("move_down", vn::Keyboard::Key::S);

        m_player = get_ecs().create_entity("Player");
        get_ecs().add<vn::component::Transform>(
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
        m_player_sprite = get_ecs().create_entity();
        get_ecs().add<vn::component::Transform>(m_player_sprite, { .parent = m_player });
        get_ecs().add<vn::component::Sprite>(
            m_player_sprite,
            {
                .texture =
                    get_textures().load("../../../examples/bomberman/assets/textures/player.png"),
                .frame_size = { 32, 32 },
                .frames_per_row = 36,
                .frames_per_col = 1,
            }
        );
    }

    void update() override {
        const float delta = get_time().get_delta_time();

        auto& player_sprite_transform = get_ecs().get<vn::component::Transform>(m_player_sprite);
        if (get_devices().get_mouse().is_wheel_triggered(vn::Mouse::Wheel::Up)) {
            player_sprite_transform.local.scale *= 1.5f;
        } else if (get_devices().get_mouse().is_wheel_triggered(vn::Mouse::Wheel::Down)) {
            player_sprite_transform.local.scale /= 1.5f;
        }

        player_input_system(get_ecs(), get_input());
        integrate_acceleration_system(get_ecs(), delta);
        integrate_velocity_system(get_ecs(), delta);
        vn::system::resolve_transform_tree(get_ecs());
    }

    void render() override {
        vn::system::draw_sprites(get_ecs(), get_renderer());
    }

private:
    vn::Entity m_player {};
    vn::Entity m_player_sprite {};
};
