#include "vinter/renderer.hpp"

#include "vinter/settings/renderer_settings.hpp"
#include "renderer_sdl.hpp"

namespace vn {
    std::unique_ptr<Renderer> Renderer::create(
        const RendererSettings& renderer_settings, 
        const Window& window
    ) {
        switch (renderer_settings.backend) {
            case RendererSettings::Backend::SDL:
                return std::make_unique<RendererSDL>(renderer_settings, window);

            case RendererSettings::Backend::SDL_GPU:
                break;
            
            case RendererSettings::Backend::OpenGL:
                break;
            }

        return nullptr;
    }

    Renderer::~Renderer() {}

    Color Renderer::get_clear_color() const { return m_clear_color; }
    void Renderer::set_clear_color(const Color color) { m_clear_color = color; }
}
