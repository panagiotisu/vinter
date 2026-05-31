#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "vinter/color.hpp"

namespace vn {
    struct RendererSettings;
    class Window;

    class Renderer {
        friend class Engine;

    public:
        static auto create(const RendererSettings& renderer_settings, const Window& window)
            -> std::unique_ptr<Renderer>;

        virtual ~Renderer() = 0;

        void set_clear_color(Color color);

        virtual void draw_pixel(glm::vec2 position, Color color) = 0;

        // TODO: Implement geometry submodule.
        virtual void draw_line(glm::vec2 start, glm::vec2 end, Color color, float weight = 1.f) = 0;

        virtual void draw_rectangle(glm::vec2 position, glm::vec2 size, Color color) = 0;

        virtual void draw_rectangle_line(
            glm::vec2 position,
            glm::vec2 size,
            Color color,
            float weight = 1.f
        ) = 0;

        virtual void
        draw_circle(glm::vec2 center, float radius, Color color, std::size_t segments = 200) = 0;

        virtual void
        draw_circle_line(glm::vec2 center, float radius, Color color, float weight = 1.f) = 0;

        virtual void draw_polygon(std::vector<glm::vec2> vertices, Color color) = 0;

    protected:
        [[nodiscard]]
        auto get_clear_color() const -> Color;

    private:
        Color m_clear_color {colors::Black};

        virtual void begin_frame() = 0;

        virtual void end_frame() = 0;
    };
} // namespace vn