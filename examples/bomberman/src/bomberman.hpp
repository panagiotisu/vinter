#pragma once
#include <iostream>

#include <vinter/engine.hpp>

#include "vinter/color.hpp"

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
    }

    void update(float /*delta*/) override {
        if (input->is_action_just_pressed("set_bg_color_red")) {
            m_background_color = vn::colors::Red;
        } else if (input->is_action_just_pressed("set_bg_color_blue")) {
            m_background_color = vn::colors::Blue;
        }

        else if (input->is_action_just_pressed("quit")) {
            quit();
        }
        // // Check action strengths.
        std::cout << input->get_action_strength("set_bg_color_blue") << '\n';
    }

    void render() override {
        renderer->set_clear_color(m_background_color);

        renderer->draw_pixel({300.f, 300.f}, vn::colors::Red);
        renderer->draw_line({0.f, 0.f}, {400.f, 500.f}, vn::colors::Yellow);
        renderer->draw_rectangle_line({200.f, 200.f}, {200.f, 100.f}, vn::colors::White);
        renderer->draw_rectangle({100.f, 100.f}, {100.f, 200.f}, vn::colors::Magenta);

        renderer->draw_circle({100.f, 100.f}, 40.f, vn::colors::Pink);
        renderer->draw_polygon(
            {{400.f, 400.f}, {450.f, 450.f}, {500.f, 350.f}, {480.f, 300.f}}, vn::colors::Yellow
        );
    }

private:
    vn::Color m_background_color {vn::colors::DarkBlue};
};
