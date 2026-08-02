#pragma once

#include <vinter/app.hpp>

class Gravitas : public vn::App {
public:
    explicit Gravitas(const vn::ProjectSettings& project_settings) : App(project_settings) {
    }

protected:
    void load() override {
        get_input().bind("spawn_planet_at_orbit", vn::Mouse::Button::Left);
        get_input().bind("speed_up_time", vn::Keyboard::Key::E);
        get_input().bind("slow_down_time", vn::Keyboard::Key::Q);
        get_input().bind("restore_time_scale", vn::Keyboard::Key::W);

        m_bodies.push_back(
            {
                .position = { get_window().get_width() / 1.2, get_window().get_height() / 1.2 },
                .mass = 100000.f,
                .radius = 50.f,
                .color = vn::colors::Yellow,
            }
        );

        get_renderer().set_clear_color(vn::colors::Black);
    }

    void update() override {
        const auto delta = get_time().get_delta_time();

        if (get_input().is_action_just_pressed("spawn_planet_at_orbit")) {
            const glm::vec2 spawn_position = get_devices().get_mouse().get_position();
            m_bodies.push_back(
                {
                    .position = spawn_position,
                    .velocity = give_orbit_velocity(spawn_position, m_bodies[0]),
                    .mass = 10.f,
                    .radius = 20.f,
                    .color = vn::colors::DarkBlue,
                }
            );
        }

        if (get_input().is_action_just_pressed("speed_up_time")) {
            get_time().set_time_scale(get_time().get_time_scale() * 2.f);
        }
        if (get_input().is_action_just_pressed("slow_down_time")) {
            get_time().set_time_scale(get_time().get_time_scale() / 2.f);
        }
        if (get_input().is_action_just_pressed("restore_time_scale")) {
            get_time().set_time_scale(1.f);
        }

        update_forces();
        update_kinematics(delta);
    }

    void render() override {
        for (const auto& body : m_bodies) {
            get_renderer().draw_circle(body.position, body.radius, body.color);
        }
    }

private:
    struct CelestialBody {
        glm::vec2 position {};
        glm::vec2 velocity {};
        glm::vec2 acceleration {};
        float mass { 1.f };
        float radius { 1.f };
        vn::Color color { vn::colors::Yellow };
    };

    void update_forces() {
        for (auto& body : m_bodies) {
            body.acceleration = { 0.f, 0.f };
        }

        for (std::size_t i = 0; i < m_bodies.size(); ++i) {
            for (std::size_t j = 0; j < m_bodies.size(); ++j) {
                if (i == j) {
                    continue;
                }

                constexpr float Epsilon = 1e-6;
                const float m_i = m_bodies[i].mass;
                const float m_j = m_bodies[j].mass;
                const glm::vec2 r = m_bodies[j].position - m_bodies[i].position;
                const float r_mag = glm::length(r) + Epsilon;
                const float r_mag3 = r_mag * r_mag * r_mag;

                const glm::vec2 norm_accel = G / r_mag3 * r;

                m_bodies[i].acceleration += m_j * norm_accel;
            }
        }
    }

    void update_kinematics(float delta) {
        for (auto& body : m_bodies) {
            body.velocity += body.acceleration * delta;
            body.position += body.velocity * delta;
        }
    }

    [[nodiscard]]
    static auto
    give_orbit_velocity(const glm::vec2& orbiter_pos, const CelestialBody& orbited) noexcept
        -> glm::vec2 {
        const glm::vec2 relative_pos = orbiter_pos - orbited.position;
        const float distance = glm::length(relative_pos);
        const float orbit_speed = std::sqrt(G * orbited.mass / distance);

        const glm::vec2 ccw_tangent = glm::normalize(glm::vec2(-relative_pos.y, relative_pos.x));

        return ccw_tangent * orbit_speed;
    }

    std::vector<CelestialBody> m_bodies {};

    static constexpr float G { 100.f };
};
