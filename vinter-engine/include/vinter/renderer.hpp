#pragma once

#include "vinter/color.hpp"

#include <glm/glm.hpp>
#include <memory>

namespace vn {
struct RendererSettings;
class Window;

class Renderer
{
    friend class Engine;

public:
    static std::unique_ptr<Renderer>
    create(RendererSettings const & renderer_settings, Window const & window);

    virtual ~Renderer() = 0;

    void set_clear_color(Color color);

protected:
    [[nodiscard]] Color get_clear_color() const;

private:
    Color m_clear_color{colors::BLACK};

    virtual void begin_frame() = 0;

    virtual void end_frame() = 0;

    virtual void draw_pixel(glm::vec2 position, Color color) = 0;

    // TODO: Implement geometry submodule.
    virtual void draw_line(glm::vec2 start, glm::vec2 end, Color color, float weight = 1.f) = 0;

    virtual void draw_rectangle(
        glm::vec2 position,
        glm::vec2 size,
        Color color,
        float weight = 1.f,
        bool filled = true
    ) = 0;

    virtual void draw_polygon(std::vector<glm::vec2> vertices, Color color) = 0;

    virtual void draw_circle(glm::vec2 center, float radius) = 0;
};
} // namespace vn