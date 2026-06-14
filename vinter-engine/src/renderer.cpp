#include "vinter/renderer.hpp"

#include <format>

#include <SDL3/SDL.h>

#include "vinter/logger.hpp"
#include "vinter/panic.hpp"
#include "vinter/settings/renderer_settings.hpp"
#include "vinter/window.hpp"

namespace vn {
    struct Renderer::Impl {
        SDL_GPUDevice* device {};
        SDL_Window* window_backend {};

        SDL_GPUCommandBuffer* cmd_buffer {};
        SDL_GPURenderPass* render_pass {};

        Impl(const Impl&) = delete;
        auto operator=(const Impl&) -> Impl& = delete;

        explicit Impl(const RendererSettings& /*settings*/, const Window& window)
            : device(SDL_CreateGPUDevice(
                  SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
                  true,
                  nullptr
              ))
            , window_backend(window.get_native_handle()) {
            Logger::info("Creating Renderer...");

            if (device == nullptr) {
                VN_FATAL("Failed creating GPU Device: ", SDL_GetError());
            }
            Logger::info("GPU Device created successfully");

            if (!SDL_ClaimWindowForGPUDevice(device, window_backend)) {
                VN_FATAL("Failed claiming window for GPU Device: ", SDL_GetError());
            }
            Logger::info("Window context claimed for GPU Device successfully");

            Logger::info("Renderer created successfully");
        }

        ~Impl() {
            Logger::info("Destroying Renderer...");
            if (device != nullptr) {
                if (window_backend != nullptr) {
                    SDL_ReleaseWindowFromGPUDevice(device, window_backend);
                    Logger::info("Window released from GPU Device successfully");
                }
                SDL_DestroyGPUDevice(device);
                Logger::info("GPU Device destroyed successfully");
            }
            Logger::info("Renderer destroyed successfully");
        }
    };

    Renderer::Renderer(const RendererSettings& settings, const Window& window)
        : m_impl(std::make_unique<Impl>(settings, window))
        , m_clear_color(settings.default_background_color) {
    }

    Renderer::~Renderer() = default;

    void Renderer::begin_frame() {
        m_impl->cmd_buffer = SDL_AcquireGPUCommandBuffer(m_impl->device);
        VN_ASSERT(
            m_impl->cmd_buffer != nullptr,
            "Error acquiring GPU command buffer. \n{}",
            SDL_GetError()
        );

        SDL_GPUTexture* swapchain_texture {};
        bool ok = SDL_WaitAndAcquireGPUSwapchainTexture(
            m_impl->cmd_buffer, m_impl->window_backend, &swapchain_texture, nullptr, nullptr
        );
        VN_ASSERT(
            ok && swapchain_texture, "Error acquiring GPU swapchain texture. \n{}", SDL_GetError()
        );

        SDL_GPUColorTargetInfo color_target {
            .texture = swapchain_texture,
            .clear_color =
                {
                    .r = m_clear_color.red(),
                    .g = m_clear_color.green(),
                    .b = m_clear_color.blue(),
                    .a = m_clear_color.alpha(),
                },
            .load_op = SDL_GPU_LOADOP_CLEAR,
            .store_op = SDL_GPU_STOREOP_STORE,
        };
        m_impl->render_pass = SDL_BeginGPURenderPass(m_impl->cmd_buffer, &color_target, 1, nullptr);
    }

    void Renderer::end_frame() {
        SDL_EndGPURenderPass(m_impl->render_pass);

        bool ok = SDL_SubmitGPUCommandBuffer(m_impl->cmd_buffer);
        VN_ASSERT(ok, "Error submitting GPU command buffer. \n{}", SDL_GetError());
    }

} // namespace vn
