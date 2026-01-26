#pragma once
#include <print>

#include <vinter/engine.hpp>

#include "vinter/keyboard.hpp"

class Bomberman : public vn::Engine {
public:
    explicit Bomberman(const vn::ProjectSettings& project_settings)
        : Engine(project_settings) {
    }

protected:
    void load() override {

    }

    void poll_events() override {

    }

    void update(float delta) override {
        if (keyboard->is_key_pressed(vn::Key::Up)) std::print("\n Pressed");
    }

    void render() override {
        renderer->set_clear_color(m_background_color);
    }

private:
    vn::Color m_background_color { vn::colors::DARK_BLUE };
};
