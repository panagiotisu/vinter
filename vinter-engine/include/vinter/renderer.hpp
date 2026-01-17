#pragma once

#include <memory>

namespace vn {
    struct RendererSettings;
    class Window;

    class Renderer {
    public:
        static std::unique_ptr<Renderer> create(const RendererSettings& renderer_settings, const Window& window);
        virtual ~Renderer() = 0;

        virtual void begin_frame() = 0;
        virtual void end_frame() = 0;
    };
} // vn