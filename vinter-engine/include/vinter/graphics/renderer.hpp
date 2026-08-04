#pragma once

#include <string>

#include "glm/glm.hpp"
#include "vinter/color.hpp"
#include "vinter/graphics/shader.hpp"
#include "vinter/settings/renderer_settings.hpp"

struct SDL_GPUDevice;
struct SDL_Window;
struct SDL_GPUCommandBuffer;
struct SDL_GPURenderPass;

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
        Renderer(const RendererSettings& settings, SDL_Window* window_handle);

        ~Renderer();

        void set_clear_color(Color color);

        void set_vsync(bool enabled);

        void draw_point(glm::vec2 position, Color color);
        void draw_line(glm::vec2 start, glm::vec2 end, float weight, Color color);
        void draw_aabb(glm::vec2 position, glm::vec2 size, Color color);
        void draw_circle(glm::vec2 center, float radius, Color color, std::size_t segment = 100);
        void draw_polygon(const std::vector<glm::vec2>& vertices, Color color);

    private:
        void begin_frame();
        void end_frame();

        static constexpr auto to_sdl_gpu_driver_name(RendererSettings::Backend rendering_backend)
            -> const char*;
        static auto to_gpu_backend_name(const char* sdl_gpu_driver_name) -> std::string;

    private:
        Color m_clear_color { colors::DarkBlue };

        SDL_GPUDevice* m_gpu_handle {};
        SDL_Window* m_window_handle {};
        SDL_GPUCommandBuffer* m_command_buffer {};
        SDL_GPURenderPass* m_render_pass {};

        Shader m_vertex_shader;
        Shader m_fragment_shader;
    };
} // namespace vn