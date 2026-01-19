#pragma once

#include <memory>

#include "vinter/renderer.hpp"

namespace vn {
    struct RendererSettings;
    class Window;

    class RendererSDLGPU final : public Renderer {
    public:
        RendererSDLGPU(const RendererSettings& renderer_settings, const Window& window);
        ~RendererSDLGPU() override;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;

        void begin_frame() override;
        void end_frame() override;
    };
} // vn