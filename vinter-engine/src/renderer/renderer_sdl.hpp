#pragma once

#include <memory>

#include "vinter/renderer.hpp"

namespace vn {
    class RendererSDL final : public Renderer {
    public:
        RendererSDL(const RendererSettings& renderer_settings, const Window& window);
        ~RendererSDL() override;

        void draw_pixel(glm::vec2 position, Color color) override;

        void draw_line(glm::vec2 start, glm::vec2 end, Color color, float weight) override;

        void draw_rectangle(glm::vec2 position, glm::vec2 size, Color color) override;

        void
        draw_rectangle_line(glm::vec2 position, glm::vec2 size, Color color, float weight) override;

        void draw_polygon(std::vector<glm::vec2> vertices, Color color) override;

        void draw_circle(
            glm::vec2 center,
            float radius,
            Color color,
            std::size_t segments = 200
        ) override;

        void draw_circle_line(glm::vec2 center, float radius, Color color, float weight) override;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;

        void begin_frame() override;

        void end_frame() override;
    };
} // namespace vn