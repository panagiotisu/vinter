#include "vinter/renderer.hpp"

#include <numbers>

#include <SDL3/SDL.h>

#include "vinter/logger.hpp"
#include "vinter/settings/renderer_settings.hpp"
#include "vinter/window.hpp"

namespace vn {
#ifdef NDEBUG
    constexpr bool EnableGPUDebug = false;
#else
    constexpr bool EnableGPUDebug = true;
#endif

    Renderer::Renderer(const RendererSettings& settings, SDL_Window* window_handle)
        : m_gpu_handle(SDL_CreateGPUDevice(
              SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXBC | SDL_GPU_SHADERFORMAT_MSL,
              EnableGPUDebug,
              to_sdl_gpu_driver_name(settings.backend)
          ))
        , m_window_handle(window_handle) {
        VN_INFO("Creating Renderer...");

        VN_ASSERT(m_gpu_handle != nullptr, "Failed creating GPU Device: {}", SDL_GetError());

        VN_INFO(
            "GPU Device created successfully: {}",
            SDL_GetStringProperty(
                SDL_GetGPUDeviceProperties(m_gpu_handle),
                SDL_PROP_GPU_DEVICE_NAME_STRING,
                "Unknown GPU"
            )
        );
        VN_INFO(
            "Selected GPU Backend: {}", to_gpu_backend_name(SDL_GetGPUDeviceDriver(m_gpu_handle))
        );

        if (!SDL_ClaimWindowForGPUDevice(m_gpu_handle, m_window_handle)) {
            VN_FATAL("Failed claiming window for GPU Device: {}", SDL_GetError());
        }
        VN_INFO("Window context claimed for GPU Device successfully");
        VN_INFO("Renderer created successfully");

        set_clear_color(settings.default_clear_color);
        set_vsync(settings.vsync);

        // Show the window (briefly hidden on startup) AFTER Renderer has been constructed, so that
        // the window does not show blank state due to non-existent renderer.
        SDL_ShowWindow(window_handle);
    }

    Renderer::~Renderer() {
        VN_INFO("Destroying Renderer...");
        if (m_gpu_handle != nullptr) {
            if (m_window_handle != nullptr) {
                SDL_ReleaseWindowFromGPUDevice(m_gpu_handle, m_window_handle);
                VN_INFO("Window released from GPU Device successfully");
            }
            SDL_DestroyGPUDevice(m_gpu_handle);
            VN_INFO("GPU Device destroyed successfully");
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

    void Renderer::set_vsync(bool enabled) {
        SDL_GPUPresentMode present_mode = SDL_GPU_PRESENTMODE_IMMEDIATE;

        const bool supports_mailbox = SDL_WindowSupportsGPUPresentMode(
            m_gpu_handle, m_window_handle, SDL_GPU_PRESENTMODE_MAILBOX
        );

        if (enabled) {
            present_mode = supports_mailbox ? SDL_GPU_PRESENTMODE_MAILBOX
                                            : SDL_GPU_PRESENTMODE_VSYNC;
        }

        if (!SDL_SetGPUSwapchainParameters(
                m_gpu_handle, m_window_handle, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, present_mode
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

    void Renderer::draw_point(glm::vec2 position, Color color) {
        // Treat the point as a tiny 1x1 pixel rectangle.
        // Offset by -0.5f to center on the pixel coordinate.
        std::vector<glm::vec2> points = {
            { position.x - 0.5f, position.y - 0.5f }, // Top-Left.
            { position.x + 0.5f, position.y - 0.5f }, // Top-Right.
            { position.x + 0.5f, position.y + 0.5f }, // Bottom-Right.
            { position.x - 0.5f, position.y + 0.5f }  // Bottom-Left.
        };

        draw_polygon(points, color);
    }

    void Renderer::draw_line(glm::vec2 start, glm::vec2 end, float weight, Color color) {
        // Avoid division by zero if the start and end points are identical.
        if (start == end) {
            draw_point(start, color);
        }

        // Calculate the direction vector and the line's perpendicular normal.
        glm::vec2 direction { end - start };

        // Rotate vector by 90 degrees (-y, x).
        glm::vec2 normal { glm::normalize(glm::vec2(-direction.y, direction.x)) };

        // Offset the points by half of the thickness on both sides of the line.
        glm::vec2 offset { normal * (weight * 0.5f) };

        // Generate the 4 corners of the thick line rectangle.
        std::vector<glm::vec2> points {
            start + offset, // Top-Left corner of the line
            end + offset,   // Top-Right corner of the line
            end - offset,   // Bottom-Right corner of the line
            start - offset  // Bottom-Left corner of the line
        };

        draw_polygon(points, color);
    }

    void Renderer::draw_aabb(glm::vec2 position, glm::vec2 size, Color color) {
        std::vector<glm::vec2> points {
            position,                                     // Top-Left.
            { position.x + size.x, position.y },          // Top-Right.
            { position.x + size.x, position.y + size.y }, // Bottom-Right.
            { position.x, position.y + size.y }           // Bottom-Left.
        };

        draw_polygon(points, color);
    }

    void Renderer::draw_circle(glm::vec2 center, float radius, Color color, std::size_t segments) {
        std::vector<glm::vec2> points;
        points.reserve(segments);

        const float increment = 2.0f * std::numbers::pi_v<float> / static_cast<float>(segments);
        for (int i = 0; i < segments; ++i) {
            float angle = static_cast<float>(i) * increment;
            points.emplace_back(
                center.x + (cosf(angle) * radius), center.y + (sinf(angle) * radius)
            );
        }

        draw_polygon(points, color);
    }

    void Renderer::draw_polygon(const std::vector<glm::vec2>& vertices, Color color) {
        if (vertices.size() < 3) {
            VN_ERROR("A valid polygon requires at least 3 vertices. Found {}", vertices.size());
            return;
        }
    }

    void Renderer::begin_frame() {
        m_command_buffer = SDL_AcquireGPUCommandBuffer(m_gpu_handle);
        VN_ASSERT(
            m_command_buffer != nullptr, "Error acquiring GPU command buffer. \n{}", SDL_GetError()
        );

        SDL_GPUTexture* swapchain_texture {};
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(
                m_command_buffer, m_window_handle, &swapchain_texture, nullptr, nullptr
            )
            && swapchain_texture != nullptr) {
            VN_FATAL("Error acquiring GPU swapchain texture. \n{}", SDL_GetError());
        }

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
        m_render_pass = SDL_BeginGPURenderPass(m_command_buffer, &color_target, 1, nullptr);
    }

    void Renderer::end_frame() {
        SDL_EndGPURenderPass(m_render_pass);

        if (!SDL_SubmitGPUCommandBuffer(m_command_buffer)) {
            VN_FATAL("Error submitting GPU command buffer. \n{}", SDL_GetError());
        }
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
