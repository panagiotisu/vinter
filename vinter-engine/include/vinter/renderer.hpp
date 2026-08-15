#pragma once

#include <string>

#include "glm/glm.hpp"
#include "vinter/color.hpp"
#include "vinter/settings/renderer_settings.hpp"

struct SDL_Renderer;
struct SDL_Window;

namespace vn {
    class Window;

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
        void draw_line(glm::vec2 start, glm::vec2 end, float weight, Color color);
        void draw_aabb(glm::vec2 position, glm::vec2 size, Color color);
        void draw_circle(glm::vec2 center, float radius, Color color, std::size_t segment = 100);
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