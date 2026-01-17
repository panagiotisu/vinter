#pragma once

#include <memory>

#include "vinter/renderer.hpp"

namespace vn {
    class RendererSDL final : public Renderer {
    public:
        RendererSDL(const RendererSettings& renderer_settings, const Window& window);
        ~RendererSDL() override;

        void begin_frame() override;

        void end_frame() override;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // vn