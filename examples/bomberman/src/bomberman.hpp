#pragma once

#include <vinter/engine.hpp>

class Bomberman : public vn::Engine {
public:
    explicit Bomberman(const vn::ProjectSettings& project_settings)
        : Engine(project_settings) {
    }

protected:
    void load() override {}
    void poll_events() override { Engine::poll_events(); }
    void update() override {}
    void render() override {}
};