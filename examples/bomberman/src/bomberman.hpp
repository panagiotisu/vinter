#pragma once

#include <vinter/app.hpp>

class Bomberman : public vn::App {
public:
    explicit Bomberman(const vn::ProjectSettings& project_settings) : App(project_settings) {
    }

protected:
    void load() override {
        get_input().bind("move_right", vn::Keyboard::Key::D);
        get_input().bind("move_left", vn::Keyboard::Key::A);
        get_input().bind("move_down", vn::Keyboard::Key::S);
        get_input().bind("move_up", vn::Keyboard::Key::W);
    }

    void update() override {
        const auto delta = get_time().get_delta_time();

        m_input_vector = get_devices().get_mouse().get_position() - m_player_pos;
        m_input_vector = glm::normalize(m_input_vector);

        if (glm::length(m_input_vector) > 0.f) {
            m_player_vel += m_input_vector * m_player_accel * delta;
        } else {
            m_player_vel -= (m_player_vel / static_cast<float>(glm::length(m_player_vel) + 1e-10))
                            * m_player_accel * delta;
        }

        m_player_pos += m_player_vel * delta;
    }

    void render() override {
        get_renderer().draw_aabb({ 100, 100 }, { 150, 300 }, vn::colors::RayWhite);
        get_renderer().draw_circle(m_player_pos, 50, vn::colors::Gold);
        get_renderer().draw_point({ 200, 200 }, vn::colors::Red);
        get_renderer().draw_line({ 100, 100 }, { 400, 400 }, 3, vn::colors::Green);
    }

private:
    glm::vec2 m_player_pos {
        static_cast<float>(get_window().get_width()) / 2,
        static_cast<float>(get_window().get_height()) / 2,
    };
    glm::vec2 m_player_vel {};
    glm::vec2 m_input_vector {};
    float m_player_accel { 100.f };
};
