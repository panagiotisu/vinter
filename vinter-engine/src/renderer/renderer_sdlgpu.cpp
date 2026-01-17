#include "renderer_sdlgpu.hpp"

#include <SDL3/SDL.h>

#include "vinter/settings/renderer_settings.hpp"

namespace vn {
    struct RendererSDLGPU::Impl {
        SDL_GPUDevice* sdl_gpu_device;
    };

    RendererSDLGPU::RendererSDLGPU(const RendererSettings &renderer_settings, const Window &window) {
    }

    RendererSDLGPU::~RendererSDLGPU() = default;

    void RendererSDLGPU::begin_frame() {
    }

    void RendererSDLGPU::end_frame() {
    }
} // vn