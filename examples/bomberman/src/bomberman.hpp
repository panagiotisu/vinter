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

        m_circly = get_ecs().create_entity("Circly");
        get_ecs().add<Transform2D>(
            m_circly,
            {
                .local = { .position = { get_window().get_width() / 2,
                                         get_window().get_height() / 2 } },
                .global = { .position = { get_window().get_width() / 2,
                                          get_window().get_height() / 2 } },

            }
        );
        get_ecs().add<Health>(m_circly);
        get_ecs().add<LinearKinematics2D>(m_circly);
        get_ecs().add<Player>(m_circly);

        vn::Entity circly_circle = get_ecs().create_entity();
        get_ecs().add<Transform2D>(circly_circle, { .parent = m_circly });
        get_ecs().add<Circle>(
            circly_circle,
            {
                .color = vn::colors::Yellow,
                .radius = 100.f,
            }
        );

        m_squary = get_ecs().create_entity("Squary");
        get_ecs().add<Transform2D>(m_squary, {});
        get_ecs().add<Health>(m_squary, {});
        get_ecs().add<AABB>(
            m_squary,
            {
                .color = vn::colors::Red,
                .size = { 100.f, 120.f },
            }
        );

        get_ecs().print_entity_components(m_circly);
        get_ecs().print_entity_components(m_squary);
    }

    void update() override {
        const float delta = get_time().get_delta_time();

        player_input_system(get_ecs(), get_input());
        integrate_acceleration_system(get_ecs(), delta);
        integrate_velocity_system(get_ecs(), delta);
        transform_hierarchy_system(get_ecs());
    }

    void render() override {
        aabb_drawing_system(get_ecs(), get_renderer());
        circle_drawing_system(get_ecs(), get_renderer());
    }

private:
    vn::Entity m_circly {};
    vn::Entity m_squary {};
    vn::Entity m_camera {};
};
