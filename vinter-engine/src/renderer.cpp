#include "vinter/renderer.hpp"

#include <numbers>

#include <SDL3/SDL.h>

#include "vinter/logger.hpp"
#include "vinter/settings/renderer_settings.hpp"
#include "vinter/window.hpp"

namespace vn {
    Renderer::Renderer(const RendererSettings& settings, const Window& window)
        : m_handle(SDL_CreateRenderer(window.get_native_handle(), nullptr)) {
        VN_INFO("Creating Renderer...");

        VN_ASSERT(m_handle, "Failed creating Graphics Device: {}", SDL_GetError());

        VN_INFO("Renderer Driver: {}", SDL_GetRendererName(m_handle));

        VN_INFO("Renderer created successfully");

        set_clear_color(settings.default_clear_color);
        set_vsync(settings.vsync);

        // Show the window (briefly hidden on startup) AFTER Renderer has been constructed, so that
        // the window does not show blank state due to non-existent renderer.
        SDL_ShowWindow(window.get_native_handle());
    }

    Renderer::~Renderer() {
        VN_INFO("Destroying Renderer...");
        if (m_handle != nullptr) {
            SDL_DestroyRenderer(m_handle);
        }
        VN_INFO("Renderer destroyed successfully");
    }

    void Renderer::set_clear_color(Color color) {
        if (m_clear_color == color) {
            return;
        }

        m_clear_color = color;
        VN_DEBUG(
            "Clear color set to: RGBA({}, {}, {}, {})",
            color.to_rgba8().r,
            color.to_rgba8().g,
            color.to_rgba8().b,
            color.to_rgba8().a
        );
    }

    void Renderer::set_vsync(RendererSettings::VSyncMode vsync) {
        RendererSettings::VSyncMode applied = vsync;

        if (!SDL_SetRenderVSync(m_handle, static_cast<int>(vsync))) {
            if (vsync == RendererSettings::VSyncMode::Adaptive
                && SDL_SetRenderVSync(
                    m_handle, static_cast<int>(RendererSettings::VSyncMode::Enabled)
                )) {
                applied = RendererSettings::VSyncMode::Enabled;
                VN_WARNING("Adaptive VSync unavailable, using enabled VSync");
            } else {
                SDL_SetRenderVSync(
                    m_handle, static_cast<int>(RendererSettings::VSyncMode::Disabled)
                );
                applied = RendererSettings::VSyncMode::Disabled;
                VN_WARNING("VSync unavailable, disabled");
            }
        }

        switch (applied) {
            case RendererSettings::VSyncMode::Enabled: {
                VN_INFO("VSync Enabled");
                return;
            }
            case RendererSettings::VSyncMode::Adaptive: {
                VN_INFO("VSync Enabled (Adaptive)");
                return;
            }
            case RendererSettings::VSyncMode::Disabled: {
                VN_INFO("VSync Disabled");
                return;
            }
        }
    }

    void Renderer::draw_point(glm::vec2 position, Color color) {
        // Treat the point as a tiny 1x1 pixel rectangle.
        // Offset by -0.5f to center on the pixel coordinate.
        std::vector<glm::vec2> points = {
            { position.x - 0.5f, position.y - 0.5f }, // Top-Left.
            { position.x + 0.5f, position.y - 0.5f }, // Top-Right.
            { position.x + 0.5f, position.y + 0.5f }, // Bottom-Right.
            { position.x - 0.5f, position.y + 0.5f }  // Bottom-Left.
        };

        draw_polygon(points, color);
    }

    void Renderer::draw_line(glm::vec2 start, glm::vec2 end, float weight, Color color) {
        // Avoid division by zero if the start and end points are identical.
        if (start == end) {
            draw_point(start, color);
        }

        // Calculate the direction vector and the line's perpendicular normal.
        glm::vec2 direction { end - start };

        // Rotate vector by 90 degrees (-y, x).
        glm::vec2 normal { glm::normalize(glm::vec2(-direction.y, direction.x)) };

        // Offset the points by half of the thickness on both sides of the line.
        glm::vec2 offset { normal * (weight * 0.5f) };

        // Generate the 4 corners of the thick line rectangle.
        std::vector<glm::vec2> points {
            start + offset, // Top-Left corner of the line
            end + offset,   // Top-Right corner of the line
            end - offset,   // Bottom-Right corner of the line
            start - offset  // Bottom-Left corner of the line
        };

        draw_polygon(points, color);
    }

    void Renderer::draw_aabb(glm::vec2 position, glm::vec2 size, Color color) {
        std::vector<glm::vec2> points {
            position,                                     // Top-Left.
            { position.x + size.x, position.y },          // Top-Right.
            { position.x + size.x, position.y + size.y }, // Bottom-Right.
            { position.x, position.y + size.y }           // Bottom-Left.
        };

        draw_polygon(points, color);
    }

    void Renderer::draw_circle(glm::vec2 center, float radius, Color color, std::size_t segments) {
        std::vector<glm::vec2> points;
        points.reserve(segments);

        const float increment = 2.0f * std::numbers::pi_v<float> / static_cast<float>(segments);
        for (int i = 0; i < segments; ++i) {
            float angle = static_cast<float>(i) * increment;
            points.emplace_back(
                center.x + (cosf(angle) * radius), center.y + (sinf(angle) * radius)
            );
        }

        draw_polygon(points, color);
    }

    void Renderer::draw_polygon(const std::vector<glm::vec2>& vertices, Color color) {
        if (vertices.size() < 3) {
            VN_ERROR("A valid polygon requires at least 3 vertices. Found {}", vertices.size());
            return;
        }

        int base_index = static_cast<int>(m_primitives.vertices.size());

        // Push all vertices into the vertex buffer.
        for (const auto& pos : vertices) {
            Vertex vertex = { pos, color };
            m_primitives.vertices.push_back(vertex);
        }

        // Build Triangle Fan Indices.
        // Anchor the fan at the very first vertex (index 0).
        // Then connect it to pairs of adjacent vertices to form triangles.
        for (int i = 1; i < vertices.size() - 1; ++i) {
            m_primitives.indices.push_back(base_index + 0);     // Anchor vertex.
            m_primitives.indices.push_back(base_index + i);     // Current vertex.
            m_primitives.indices.push_back(base_index + i + 1); // Next vertex.
        }
    }

    void Renderer::begin_frame() {
        clear();
    }

    void Renderer::end_frame() {
        flush_primitives();
        SDL_RenderPresent(m_handle);
    }

    void Renderer::clear() {
        m_primitives.clear();

        set_draw_color(m_clear_color);
        SDL_RenderClear(m_handle);
    }

    void Renderer::set_draw_color(Color color) {
        ColorRGBA8 rgba8 { color.to_rgba8() };
        SDL_SetRenderDrawColor(m_handle, rgba8.r, rgba8.g, rgba8.b, rgba8.a);
    }

    void Renderer::flush_primitives() {
        if (m_primitives.is_empty()) {
            return;
        }

        // Convert Renderer::Vertex to their SDL_Vertex equivalents.
        static_assert(sizeof(Vertex) == sizeof(SDL_Vertex));
        const SDL_Vertex* sdl_vertices {
            reinterpret_cast<const SDL_Vertex*>(m_primitives.vertices.data())
        };

        // Issue a draw call for all primitives.
        SDL_RenderGeometry(
            m_handle,
            nullptr,
            sdl_vertices,
            static_cast<int>(m_primitives.vertices.size()),
            m_primitives.indices.data(),
            static_cast<int>(m_primitives.indices.size())
        );
    }
} // namespace vn
