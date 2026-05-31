#pragma once
#include <iostream>

#include <vinter/engine.hpp>

#include "vinter/color.hpp"
#include "vinter/input/mouse.hpp"

class Bomberman : public vn::Engine {
public:
    explicit Bomberman(const vn::ProjectSettings& project_settings) : Engine(project_settings) {
    }

protected:
    void load() override {
        input->bind("set_bg_color_red", vn::Mouse::Button::Left);
        input->bind("set_bg_color_red", vn::Keyboard::Key::Space);

        input->bind("set_bg_color_blue", vn::Mouse::Button::Right);
        input->bind("set_bg_color_blue", vn::Gamepad::Button::East);
        input->bind("set_bg_color_blue", vn::Gamepad::Axis::LeftStickLeft, 1);

        input->bind("quit", vn::Keyboard::Key::Esc);

        input->bind("add_segments", vn::Mouse::Wheel::Up);
        input->bind("remove_segments", vn::Mouse::Wheel::Down);
    }

    void update(float /*delta*/) override {
        if (input->is_action_just_pressed("set_bg_color_red")) {
            m_background_color = vn::colors::Red;
        } else if (input->is_action_just_pressed("set_bg_color_blue")) {
            m_background_color = vn::colors::Blue;
        } else if (input->is_action_just_pressed("add_segments")) {
            m_segment_count++;
        } else if (input->is_action_just_pressed("remove_segments")) {
            m_segment_count--;
        } else if (input->is_action_just_pressed("quit")) {
            quit();
        }
        // // Check action strengths.
        std::cout << input->get_action_strength("set_bg_color_blue") << '\n';

        m_circle_pos = devices->get_mouse().get_position();
    }

    void render() override {
        renderer->set_clear_color(m_background_color);
    }

private:
    vn::Color m_background_color {vn::colors::DarkBlue};
    glm::vec2 m_circle_pos {};
    float m_circle_radius {40.f};
    std::size_t m_segment_count {100};
};
