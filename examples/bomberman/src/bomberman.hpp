#pragma once
#include <print>
#include <vinter/engine.hpp>

class Bomberman : public vn::Engine {
public:
    explicit Bomberman(const vn::ProjectSettings& project_settings)
        : Engine(project_settings) {
    }

protected:
    void load() override {
        input->bind("toggle_cursor_visibility", vn::MouseButton::Left);
        input->bind("toggle_cursor_visibility", vn::Key::Space);
    }

    void poll_events() override {

    }

    void update(float delta) override {
        if (input->is_action_just_pressed("toggle_cursor_visibility")) {
            if (mouse->is_cursor_visible()) mouse->set_cursor_visible(false);
            else mouse->set_cursor_visible(true);
        }
    }

    void render() override {
        renderer->set_clear_color(m_background_color);
    }

private:
    vn::Color m_background_color { vn::colors::DARK_BLUE };
};
