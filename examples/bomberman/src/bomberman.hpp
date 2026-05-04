#pragma once
#include <iostream>

#include <vinter/engine.hpp>

class Bomberman : public vn::Engine {
public:
    explicit Bomberman(const vn::ProjectSettings& projectSettings) : Engine(projectSettings) {
    }

protected:
    void Load() override {
        m_input->Bind("set_bg_color_red", vn::Mouse::Button::Left);
        m_input->Bind("set_bg_color_red", vn::Keyboard::Key::Space);

        m_input->Bind("set_bg_color_blue", vn::Mouse::Button::Right);
        m_input->Bind("set_bg_color_blue", vn::Gamepad::Button::East);
        m_input->Bind("set_bg_color_blue", vn::Gamepad::Axis::LeftStickLeft, 1);

        m_input->Bind("quit", vn::Keyboard::Key::Esc);
    }

    void Update(float /*delta*/) override {
        if (m_input->IsActionJustPressed("set_bg_color_red")) {
            m_backgroundColor = vn::Color::Red();
        } else if (m_input->IsActionJustPressed("set_bg_color_blue")) {
            m_backgroundColor = vn::Color::Blue();
        }

        else if (m_input->IsActionJustPressed("quit")) {
            Quit();
        }
        // // Check action strengths.
        std::cout << m_input->GetActionStrength("set_bg_color_blue") << '\n';
    }

    void Render() override {
        m_renderer->SetClearColor(m_backgroundColor);

        m_renderer->DrawPixel({300.f, 300.f}, vn::Color::Red());
        m_renderer->DrawLine({0.f, 0.f}, {400.f, 500.f}, vn::Color::Yellow());
        m_renderer->DrawRectangle({200.f, 200.f}, {200.f, 100.f}, vn::Color::White());
        m_renderer->DrawRectangleFilled({100.f, 100.f}, {100.f, 200.f}, vn::Color::Magenta());
    }

private:
    vn::Color m_backgroundColor {vn::Color::DarkBlue()};
};
