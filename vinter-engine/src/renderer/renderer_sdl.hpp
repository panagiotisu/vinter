#pragma once

#include <memory>

#include "vinter/renderer.hpp"

namespace vn {
    class RendererSDL final : public Renderer {
    public:
        RendererSDL(const RendererSettings& rendererSettings, const Window& window);
        ~RendererSDL() override;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;

        void BeginFrame() override;

        void EndFrame() override;

        void DrawPixel(glm::vec2 position, Color color) override;

        void DrawLine(glm::vec2 start, glm::vec2 end, Color color, float weight = 1.f) override;

        void DrawRectangle(
            glm::vec2 position,
            glm::vec2 size,
            Color color,
            float weight = 1.f,
            bool filled = true
        ) override;

        void DrawPolygon(std::vector<glm::vec2> vertices, Color color) override;

        void DrawCircle(glm::vec2 center, float radius) override;
    };
} // namespace vn