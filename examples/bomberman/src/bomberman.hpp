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

    }

    void poll_events() override {

    }

    void update(float delta) override {
        if (mouse->get_scroll_delta() > 0) {
            m_background_color = vn::colors::RED;
        } else if (mouse->get_scroll_delta() < 0) {
            m_background_color = vn::colors::GREEN;
        } else {
            m_background_color = vn::colors::DARK_BLUE;
        }
    }

    void render() override {
        renderer->set_clear_color(m_background_color);
    }

private:
    vn::Color m_background_color { vn::colors::DARK_BLUE };
};
