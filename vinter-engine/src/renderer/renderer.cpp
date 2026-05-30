#include "vinter/renderer.hpp"

#include "renderer_sdl.hpp"
#include "vinter/settings/renderer_settings.hpp"

namespace vn {
    auto Renderer::create(const RendererSettings& renderer_settings, const Window& window)
        -> std::unique_ptr<Renderer> {
        switch (renderer_settings.backend) {
            case RendererSettings::Backend::SDL:
                return std::make_unique<RendererSDL>(renderer_settings, window);

            case RendererSettings::Backend::SdlGpu:
            case RendererSettings::Backend::OpenGL: break;
        }

        return nullptr;
    }

    Renderer::~Renderer() = default;

    auto Renderer::get_clear_color() const -> Color {
        return m_clear_color;
    }

    void Renderer::set_clear_color(const Color color) {
        m_clear_color = color;
    }
} // namespace vn
