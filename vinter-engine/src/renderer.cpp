#include "vinter/renderer.hpp"

#include <SDL3/SDL.h>

#include "vinter/logger.hpp"
#include "vinter/settings/renderer_settings.hpp"
#include "vinter/window.hpp"

namespace vn {
    Renderer::Renderer(const RendererSettings& settings, const Window& window)
        : m_renderer_backend(SDL_CreateRenderer(
              window.get_native_handle(),
              to_sdl_gpu_driver_name(settings.backend)
          )) {
        VN_INFO("Creating Renderer...");

        if (m_renderer_backend == nullptr) {
            VN_FATAL("Failed creating Graphics Device: {}", SDL_GetError());
        }
        VN_INFO(
            "Graphics Device created successfully: {}",
            SDL_GetStringProperty(
                SDL_GetGPUDeviceProperties(SDL_GetGPURendererDevice(m_renderer_backend)),
                SDL_PROP_GPU_DEVICE_NAME_STRING,
                "Unknown GPU"
            )
        );
        VN_INFO(
            "Selected GPU Backend: {}", to_gpu_backend_name(SDL_GetRendererName(m_renderer_backend))
        );

        VN_INFO("Renderer created successfully");

        set_clear_color(settings.default_clear_color);
        set_vsync(settings.vsync);

        // Show the window (briefly hidden on startup) AFTER Renderer has been constructed, so that
        // the window does not show blank state due to non-existent renderer.
        SDL_ShowWindow(window.get_native_handle());
    }

    Renderer::~Renderer() {
        VN_INFO("Destroying Renderer...");
        if (m_renderer_backend != nullptr) {
            SDL_DestroyRenderer(m_renderer_backend);
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
        if (!SDL_SetRenderVSync(m_renderer_backend, static_cast<int>(vsync))) {
            VN_FATAL("Failed to set VSync: {}", SDL_GetError());
        }

        switch (vsync) {
            case RendererSettings::VSyncMode::Enabled: {
                VN_INFO("VSync Enabled");
                break;
            }
            case RendererSettings::VSyncMode::Adaptive: {
                VN_INFO("VSync Enabled (Adaptive)");
                break;
            }
            case RendererSettings::VSyncMode::Disabled: {
                VN_INFO("VSync Disabled");
                break;
            }
        }
    }

    void Renderer::begin_frame() {
        clear();
    }

    void Renderer::end_frame() {
        SDL_RenderPresent(m_renderer_backend);
    }

    void Renderer::clear() {
        ColorRGBA8 rgba8 { m_clear_color.to_rgba8() };
        SDL_SetRenderDrawColor(m_renderer_backend, rgba8.r, rgba8.g, rgba8.b, rgba8.a);
        SDL_RenderClear(m_renderer_backend);
    }

    auto Renderer::to_sdl_gpu_driver_name(RendererSettings::Backend backend) -> const char* {
        switch (backend) {
            case RendererSettings::Backend::Vulkan: return "vulkan";
            case RendererSettings::Backend::Direct3D12: return "direct3d12";
            case RendererSettings::Backend::Metal: return "metal";
            case RendererSettings::Backend::Automatic: return nullptr;
        }
        return nullptr;
    }

    auto Renderer::to_gpu_backend_name(const char* sdl_gpu_driver_name) -> std::string {
        if (strcmp(sdl_gpu_driver_name, "vulkan") == 0) {
            return "Vulkan";
        }
        if (strcmp(sdl_gpu_driver_name, "direct3d12") == 0) {
            return "Direct3D12";
        }
        if (strcmp(sdl_gpu_driver_name, "metal") == 0) {
            return "Metal";
        }
        return {};
    }

} // namespace vn
