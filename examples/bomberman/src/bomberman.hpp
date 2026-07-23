#pragma once

#include <vinter/app.hpp>

class Bomberman : public vn::App {
public:
    explicit Bomberman(const vn::ProjectSettings& project_settings) : App(project_settings) {
    }

protected:
    void load() override {
    }

    void update() override {
        if (get_devices().get_mouse().is_button_just_pressed(vn::Mouse::Button::Left)) {
            get_renderer().set_clear_color(vn::colors::Gold);
        } else if (get_devices().get_mouse().is_wheel_triggered(vn::Mouse::Wheel::Up)) {
            get_renderer().set_clear_color(vn::colors::Red);
        }
    }

    void render() override {
    }

private:
};
