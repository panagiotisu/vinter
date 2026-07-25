#pragma once

#include <string>

#include "vinter/color.hpp"
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
        Renderer(const RendererSettings& settings, const Window& window);
        ~Renderer();

        void set_clear_color(Color color);

        void set_vsync(bool enabled);

    private:
        void begin_frame();
        void end_frame();

        [[nodiscard]]
        static auto to_sdl_gpu_shader_format(RendererSettings::Backend rendering_backend)
            -> std::uint32_t;

        [[nodiscard]]
        static auto to_sdl_gpu_driver_name(RendererSettings::Backend backend) -> const char*;

        [[nodiscard]]
        static auto to_gpu_backend_name(const char* sdl_gpu_driver_name) -> std::string;

    private:
        Color m_clear_color { colors::DarkBlue };
        bool m_vsync_enabled { true };

        SDL_GPUDevice* m_device {};
        SDL_Window* m_window_backend {};
        SDL_GPUCommandBuffer* m_cmd_buffer {};
        SDL_GPURenderPass* m_render_pass {};
    };
} // namespace vn