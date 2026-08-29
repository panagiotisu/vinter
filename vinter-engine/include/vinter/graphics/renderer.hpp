#pragma once

#include <glm/glm.hpp>

#include "vinter/color.hpp"
#include "vinter/settings/renderer_settings.hpp"

struct SDL_Renderer;
struct SDL_Window;

namespace vn {
    class Window;
    struct Line;
    struct AABB;
    struct Circle;

    class Renderer {
        friend class App;

    public:
        struct Vertex {
            glm::vec2 position {};
            Color color {};
            glm::vec2 tex_coords {};
        };

    public:
        Renderer(const RendererSettings& settings, const Window& window);
        ~Renderer();

        void set_clear_color(Color color);
        void set_vsync(RendererSettings::VSyncMode vsync);

        void draw_point(glm::vec2 position, Color color);
        void draw_line(const Line& line, float weight, Color color);
        void draw_aabb(const AABB& aabb, Color color);
        void draw_circle(const Circle& circle, Color color, std::size_t segment = 100);
        void draw_polygon(const std::vector<glm::vec2>& vertices, Color color);

        [[nodiscard]]
        SDL_Renderer* get_native_handle() const;

    private:
        void begin_frame();
        void end_frame();

        void clear();
        void set_draw_color(Color color);

        void flush_primitives();

    private:
        Color m_clear_color { colors::DarkBlue };

        struct VertexArray {
            std::vector<Vertex> vertices {};
            std::vector<int> indices {};

            void clear() {
                vertices.clear();
                indices.clear();
            }

            [[nodiscard]]
            constexpr auto is_empty() const noexcept -> bool {
                return vertices.empty() && indices.empty();
            }
        };

        VertexArray m_primitives {};

        SDL_Renderer* m_handle {};
    };
} // namespace vn