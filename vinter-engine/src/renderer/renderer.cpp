#include "vinter/renderer.hpp"

#include "renderer_sdl.hpp"
#include "vinter/settings/renderer_settings.hpp"

namespace vn {
    auto Renderer::Create(const RendererSettings& rendererSettings, const Window& window)
        -> std::unique_ptr<Renderer> {
        switch (rendererSettings.backend) {
            case RendererSettings::Backend::SDL:
                return std::make_unique<RendererSDL>(rendererSettings, window);

            case RendererSettings::Backend::SdlGpu:
            case RendererSettings::Backend::OpenGL: break;
        }

        return nullptr;
    }

    Renderer::~Renderer() {
    }

    auto Renderer::GetClearColor() const -> Color {
        return m_clearColor;
    }

    void Renderer::SetClearColor(const Color color) {
        m_clearColor = color;
    }
} // namespace vn
