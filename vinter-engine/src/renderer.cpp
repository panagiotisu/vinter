#include "vinter/renderer.hpp"

#include <format>

#include <SDL3/SDL.h>

#include "vinter/panic.hpp"
#include "vinter/settings/renderer_settings.hpp"
#include "vinter/window.hpp"

namespace vn {
    struct Renderer::Impl {
        SDL_GPUDevice* gpu_device {};
        SDL_Window* window_backend {};

        Impl(const Impl&) = delete;
        auto operator=(const Impl&) -> Impl& = delete;
        Impl(Impl&&) = delete;
        auto operator=(Impl&&) -> Impl& = delete;

        explicit Impl(const RendererSettings& settings, const Window& window)
            : gpu_device(SDL_CreateGPUDevice(
                  SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
                  true,
                  nullptr
              ))
            , window_backend(window.get_native_handle()) {
            if (gpu_device == nullptr) {
                VN_FATAL("Failed creating GPU Device: ", SDL_GetError());
            }

            if (!SDL_ClaimWindowForGPUDevice(gpu_device, window_backend)) {
                VN_FATAL("Failed claiming window for GPU Device: ", SDL_GetError());
            }
        }

        ~Impl() {
            SDL_ReleaseWindowFromGPUDevice(gpu_device, window_backend);
            SDL_DestroyGPUDevice(gpu_device);
        }
    };

    Renderer::Renderer(const RendererSettings& settings, const Window& window)
        : m_impl(std::make_unique<Impl>(settings, window)) {
    }

    Renderer::~Renderer() = default;

} // namespace vn
