#pragma once

#include <vinter/app.hpp>

class Bomberman : public vn::App {
public:
    explicit Bomberman(const vn::ProjectSettings& project_settings) : App(project_settings) {
    }

protected:
    void load() override {
        CelestialBody altair = {
            .position = { get_window().get_width() / 2, get_window().get_height() / 2 },
            .mass = 1000.f,
            .radius = 50.f,
            .color = vn::colors::Yellow,
        };

        CelestialBody fortuna = {
            .position = { altair.position.x + 100.f, altair.position.y },
            .velocity = { 0.f, 10.f },
            .mass = 10.f,
            .radius = 10.f,
            .color = vn::colors::DarkBlue,
        };

        m_bodies.push_back(altair);
        m_bodies.push_back(fortuna);

        get_renderer().set_clear_color(vn::colors::Black);
    }

    void update() override {
        const auto delta = get_time().get_delta_time();
        update_forces();
        update_kinematics(delta);
    }

    void render() override {
        for (const auto& body : m_bodies) {
            get_renderer().draw_circle(body.position, body.radius, body.color);
        }
    }

private:
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
                const float r_mag = glm::length(r + Epsilon);
                const float r_mag3 = r_mag * r_mag * r_mag;

                const glm::vec2 norm_accel = G / r_mag3 * r;

                m_bodies[i].acceleration = m_j * norm_accel;
                m_bodies[j].acceleration = -m_i * norm_accel;
            }
        }
    }

    void update_kinematics(float delta) {
        for (auto& body : m_bodies) {
            body.velocity += body.acceleration * delta;
            body.position += body.velocity * delta;
        }
    }

    struct CelestialBody {
        glm::vec2 position {};
        glm::vec2 velocity {};
        glm::vec2 acceleration {};
        float mass { 1.f };
        float radius { 1.f };
        vn::Color color { vn::colors::Yellow };
    };

    std::vector<CelestialBody> m_bodies {};

    static constexpr float G { 1e-6 };
};
