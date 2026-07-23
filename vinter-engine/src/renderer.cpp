#include "vinter/renderer.hpp"

#include <SDL3/SDL.h>

#include "vinter/logger.hpp"
#include "vinter/settings/renderer_settings.hpp"
#include "vinter/window.hpp"

namespace vn {
    Renderer::Renderer(const RendererSettings& settings, const Window& window)
        : m_device(SDL_CreateGPUDevice(
              to_sdl_gpu_shader_format(settings.backend),
              true,
              to_sdl_gpu_driver_name(settings.backend)
          ))
        , m_window_backend(window.get_native_handle()) {
        VN_INFO("Creating Renderer...");

        if (m_device == nullptr) {
            VN_FATAL("Failed creating GPU Device: {}", SDL_GetError());
        }
        VN_INFO(
            "GPU Device created successfully: {}",
            SDL_GetStringProperty(
                SDL_GetGPUDeviceProperties(m_device), SDL_PROP_GPU_DEVICE_NAME_STRING, "Unknown GPU"
            )
        );
        VN_INFO("Selected GPU Backend: {}", to_gpu_backend_name(SDL_GetGPUDeviceDriver(m_device)));

        if (!SDL_ClaimWindowForGPUDevice(m_device, m_window_backend)) {
            VN_FATAL("Failed claiming window for GPU Device: {}", SDL_GetError());
        }
        VN_INFO("Window context claimed for GPU Device successfully");
        VN_INFO("Renderer created successfully");

        set_clear_color(settings.default_clear_color);
        set_vsync(settings.vsync_enabled);
    }

    Renderer::~Renderer() {
        VN_INFO("Destroying Renderer...");
        if (m_device != nullptr) {
            if (m_window_backend != nullptr) {
                SDL_ReleaseWindowFromGPUDevice(m_device, m_window_backend);
                VN_INFO("Window released from GPU Device successfully");
            }
            SDL_DestroyGPUDevice(m_device);
            VN_INFO("GPU Device destroyed successfully");
        }
        VN_INFO("Renderer destroyed successfully");
    }

    void Renderer::set_clear_color(Color color) {
        m_clear_color = color;
        VN_DEBUG(
            "Clear color set to: RGBA({}, {}, {}, {})",
            color.to_rgba8().r,
            color.to_rgba8().g,
            color.to_rgba8().b,
            color.to_rgba8().a
        );
    }

    auto Renderer::vsync_enabled() const noexcept -> bool {
        return m_vsync_enabled;
    }

    void Renderer::set_vsync(bool enabled) {
        m_vsync_enabled = false;
        SDL_GPUPresentMode present_mode { SDL_GPU_PRESENTMODE_IMMEDIATE };

        const bool supports_mailbox { SDL_WindowSupportsGPUPresentMode(
            m_device, m_window_backend, SDL_GPU_PRESENTMODE_MAILBOX
        ) };

        if (enabled) {
            present_mode = supports_mailbox ? SDL_GPU_PRESENTMODE_MAILBOX
                                            : SDL_GPU_PRESENTMODE_VSYNC;
            m_vsync_enabled = true;
        }

        if (!SDL_SetGPUSwapchainParameters(
                m_device, m_window_backend, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, present_mode
            )) {
            VN_FATAL("Failed setting up GPU swapchain parameters: {}", SDL_GetError());
        }

        switch (present_mode) {
            case SDL_GPU_PRESENTMODE_IMMEDIATE: {
                VN_INFO("VSync disabled");
                break;
            }
            case SDL_GPU_PRESENTMODE_VSYNC: {
                VN_INFO("VSync enabled");
                break;
            }
            case SDL_GPU_PRESENTMODE_MAILBOX: {
                VN_INFO("VSync enabled (Mailbox)");
                break;
            }
        }
    }

    void Renderer::begin_frame() {
        m_cmd_buffer = SDL_AcquireGPUCommandBuffer(m_device);
        VN_ASSERT(
            m_cmd_buffer != nullptr, "Error acquiring GPU command buffer. \n{}", SDL_GetError()
        );

        SDL_GPUTexture* swapchain_texture {};
        const bool ok = SDL_WaitAndAcquireGPUSwapchainTexture(
            m_cmd_buffer, m_window_backend, &swapchain_texture, nullptr, nullptr
        );
        VN_ASSERT(
            ok && swapchain_texture, "Error acquiring GPU swapchain texture. \n{}", SDL_GetError()
        );

        const SDL_GPUColorTargetInfo color_target {
            .texture = swapchain_texture,
            .clear_color = { 
                .r = m_clear_color.red(),
                .g = m_clear_color.green(),
                .b = m_clear_color.blue(),
                .a = m_clear_color.alpha(), 
            },
            .load_op = SDL_GPU_LOADOP_CLEAR,
            .store_op = SDL_GPU_STOREOP_STORE,
        };
        m_render_pass = SDL_BeginGPURenderPass(m_cmd_buffer, &color_target, 1, nullptr);
    }

    void Renderer::end_frame() {
        SDL_EndGPURenderPass(m_render_pass);

        const bool ok = SDL_SubmitGPUCommandBuffer(m_cmd_buffer);
        VN_ASSERT(ok, "Error submitting GPU command buffer. \n{}", SDL_GetError());
    }

    auto Renderer::to_sdl_gpu_shader_format(RendererSettings::Backend rendering_backend)
        -> SDL_GPUShaderFormat {
        SDL_GPUShaderFormat sdl_gpu_shader_format {};

        switch (rendering_backend) {
            case RendererSettings::Backend::Vulkan:
                sdl_gpu_shader_format |= SDL_GPU_SHADERFORMAT_SPIRV;
                break;
            case RendererSettings::Backend::Direct3D12:
                sdl_gpu_shader_format |= SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL
                                         | SDL_GPU_SHADERFORMAT_DXBC;
                break;
            case RendererSettings::Backend::Metal:
                sdl_gpu_shader_format |= SDL_GPU_SHADERFORMAT_MSL | SDL_GPU_SHADERFORMAT_METALLIB;
                break;
            case RendererSettings::Backend::Automatic:
                sdl_gpu_shader_format |= SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL
                                         | SDL_GPU_SHADERFORMAT_DXBC | SDL_GPU_SHADERFORMAT_MSL
                                         | SDL_GPU_SHADERFORMAT_METALLIB;
                break;
        }
        return sdl_gpu_shader_format;
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
