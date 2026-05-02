#pragma once

#include "vinter/renderer.hpp"

#include <memory>

namespace vn {
class RendererSDL final : public Renderer
{
public:
    RendererSDL(RendererSettings const & renderer_settings, Window const & window);
    ~RendererSDL() override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;

    void begin_frame() override;

    void end_frame() override;

    void draw_pixel(glm::vec2 position, Color color) override;

    void draw_line(glm::vec2 start, glm::vec2 end, Color color, float weight = 1.f) override;

    void draw_rectangle(
        glm::vec2 position,
        glm::vec2 size,
        Color color,
        float weight = 1.f,
        bool filled = true
    ) override;

    void draw_polygon(std::vector<glm::vec2> vertices, Color color) override;

    void draw_circle(glm::vec2 center, float radius) override;
};
} // namespace vn