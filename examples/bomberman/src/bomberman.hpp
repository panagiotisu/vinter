#pragma once
#include <vinter/engine.hpp>
#include <iostream>

class Bomberman : public vn::Engine {
public:
    explicit Bomberman(const vn::ProjectSettings& project_settings)
        : Engine(project_settings) {
    }

protected:
    void load() override {
        input->bind("set_bg_color_red", vn::Mouse::Button::Left);
        input->bind("set_bg_color_red", vn::Keyboard::Key::Space);

        // input->bind("set_bg_color_blue", vn::Mouse::Button::Right);
        // input->bind("set_bg_color_blue", vn::Gamepad::Button::East);
        // input->bind("set_bg_color_blue", vn::Gamepad::Axis::LeftStickLeft);

        input->bind("quit", vn::Keyboard::Key::Esc);
    }

    void update(float delta) override {
        if (input->is_action_just_pressed("set_bg_color_red")) {
            m_background_color = vn::colors::RED;
        }
        // else if (input->is_action_just_pressed("set_bg_color_blue")) {
        //     m_background_color = vn::colors::BLUE;
        // }
        //
        // // Check action strengths.
        std::cout << devices->get_gamepad()->get_axis_strength(vn::Gamepad::Axis::LeftStickLeft) << std::endl;
    }

    void render() override {
        renderer->set_clear_color(m_background_color);
    }

private:
    vn::Color m_background_color { vn::colors::DARK_BLUE };
};
