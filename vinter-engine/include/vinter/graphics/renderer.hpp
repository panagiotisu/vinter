#pragma once

#include <string_view>

#include <glm/glm.hpp>

#include "vinter/assets/font_manager.hpp"
#include "vinter/color.hpp"
#include "vinter/graphics/font.hpp"
#include "vinter/settings/renderer_settings.hpp"

struct SDL_Renderer;
struct SDL_Window;

namespace vn {
    class Window;
    struct Line;
    struct AABB;
    struct Circle;
    class Texture;
    class TextureManager;

    class Renderer {
        friend class App;

    public:
        struct Vertex {
            glm::vec2 position {};
            Color color {};
            glm::vec2 tex_coords {};
        };

    public:
        Renderer(
            const RendererSettings& settings,
            const Window& window,
            TextureManager& texture_manager,
            FontManager& font_manager
        );
        ~Renderer();

        void set_clear_color(Color color);
        void set_vsync(RendererSettings::VSyncMode vsync);

        void draw_point(glm::vec2 position, Color color);
        void draw_line(const Line& line, float weight, Color color);
        void draw_aabb(const AABB& aabb, Color color);
        void draw_circle(const Circle& circle, Color color, std::size_t segment = 100);
        void draw_polygon(const std::vector<glm::vec2>& vertices, Color color);

        void draw_texture(
            const Texture& texture,
            const AABB& src_aabb,
            const AABB& dest_aabb,
            float angle_deg = {},
            glm::vec2 pivot = {},
            glm::bvec2 flip = {}
        ) const;

        void draw_text(
            const std::string& text,
            Font font,
            glm::vec2 position,
            Color color,
            float angle_deg = {},
            float scale = 1.f,
            glm::vec2 pivot = {},
            glm::bvec2 flip = {}
        );

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

        SDL_Renderer* m_native_handle {};
        TextureManager& m_texture_manager;
        FontManager& m_font_manager;
    };
} // namespace vn