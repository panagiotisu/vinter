#pragma once

#include <memory>

#include "vinter/renderer.hpp"

namespace vn {
    class RendererSDL final : public Renderer {
    public:
        RendererSDL(const RendererSettings& rendererSettings, const Window& window);
        ~RendererSDL() override;

        void DrawPixel(glm::vec2 position, Color color) override;

        void DrawLine(glm::vec2 start, glm::vec2 end, Color color, float weight) override;

        void DrawRectangle(glm::vec2 position, glm::vec2 size, Color color) override;

        void
        DrawRectangleLine(glm::vec2 position, glm::vec2 size, Color color, float weight) override;

        void DrawPolygon(std::vector<glm::vec2> vertices, Color color) override;

        void DrawCircle(
            glm::vec2 center,
            float radius,
            Color color,
            std::size_t segments = 200
        ) override;

        void DrawCircleLine(glm::vec2 center, float radius, Color color, float weight) override;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;

        void BeginFrame() override;

        void EndFrame() override;
    };
} // namespace vn