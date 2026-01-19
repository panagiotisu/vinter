#pragma once

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

    }

    void render() override {
        renderer->set_clear_color(vn::colors::BLUE);
    }
};
