#pragma once
#include <vinter/engine.hpp>

class Bomberman : public vn::Engine {
public:
    explicit Bomberman(const vn::ProjectSettings& project_settings)
        : Engine(project_settings) {
    }

    class Transform2D : public vn::Component {
    public:
        Transform2D(const glm::vec2 pos, const float rot)
            : m_position(pos)
            , m_rotation(rot) {
        }

    private:
        glm::vec2 m_position { 0, 0 };
        glm::vec2 m_scale { 1, 1};
        float m_rotation { 0 };
    };

    vn::GameObject root;

protected:
    void load() override {
        input->bind("set_bg_color_red", vn::MouseButton::Left);
        input->bind("set_bg_color_red", vn::Key::Space);
        input->bind("set_bg_color_blue", vn::Key::T);

        root.add_component(Transform2D({0,0}, 0));
    }

    void update(float delta) override {
        if (input->is_action_just_pressed("set_bg_color_red")) {
            m_background_color = vn::colors::RED;
        }
        else if (input->is_action_just_pressed("set_bg_color_blue")) {
            m_background_color = vn::colors::BLUE;
        }
    }

    void render() override {
        renderer->set_clear_color(m_background_color);
    }

private:
    vn::Color m_background_color { vn::colors::DARK_BLUE };
};
