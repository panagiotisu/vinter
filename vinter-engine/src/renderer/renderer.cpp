#include "vinter/renderer.hpp"

#include "vinter/settings/renderer_settings.hpp"
#include "renderer_sdl.hpp"
#include "renderer_sdlgpu.hpp"

namespace vn {
    std::unique_ptr<Renderer> Renderer::create(const RendererSettings &renderer_settings, const Window &window) {
        switch (renderer_settings.backend) {
            case RendererSettings::Backend::SDL:
                return std::make_unique<RendererSDL>(renderer_settings, window);

            case RendererSettings::Backend::SDL_GPU:
                return std::make_unique<RendererSDLGPU>(renderer_settings, window);
        }

        return nullptr;
    }

    Renderer::~Renderer() {}

    Color Renderer::get_clear_color() const { return m_clear_color; }
    void Renderer::set_clear_color(const Color color) { m_clear_color = color; }
}
