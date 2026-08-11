#pragma once

#include <vinter/app.hpp>

class Bomberman : public vn::App {
public:
    explicit Bomberman(const vn::ProjectSettings& project_settings) : App(project_settings) {
    }

protected:
    void load() override {
        m_bomberman = get_ecs().create_entity("Bomberman");
        get_ecs().add<Transform2D>(m_bomberman, {});
        get_ecs().add<Health>(m_bomberman, {});
    }

    void update() override {
    }

    void render() override {
    }

private:
    vn::Entity m_bomberman;

    struct Transform2D {
        glm::vec2 position {};
        glm::vec2 scale {};
        float rotation {};
    };

    struct Health {
        float max_health { 100.f };
        float regeneration { 1.f };
        float current_health { 1.f };
    };
};
