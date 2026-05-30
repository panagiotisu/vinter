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
        static auto Create(const RendererSettings& rendererSettings, const Window& window)
            -> std::unique_ptr<Renderer>;

        virtual ~Renderer() = 0;

        void SetClearColor(Color color);

        virtual void DrawPixel(glm::vec2 position, Color color) = 0;

        // TODO: Implement geometry submodule.
        virtual void DrawLine(glm::vec2 start, glm::vec2 end, Color color, float weight = 1.f) = 0;

        virtual void DrawRectangle(glm::vec2 position, glm::vec2 size, Color color) = 0;

        virtual void
        DrawRectangleLine(glm::vec2 position, glm::vec2 size, Color color, float weight = 1.f) = 0;

        virtual void
        DrawCircle(glm::vec2 center, float radius, Color color, std::size_t segments = 200) = 0;

        virtual void
        DrawCircleLine(glm::vec2 center, float radius, Color color, float weight = 1.f) = 0;

        virtual void DrawPolygon(std::vector<glm::vec2> vertices, Color color) = 0;

    protected:
        [[nodiscard]]
        auto GetClearColor() const -> Color;

    private:
        Color m_clearColor {colors::Black};

        virtual void BeginFrame() = 0;

        virtual void EndFrame() = 0;
    };
} // namespace vn