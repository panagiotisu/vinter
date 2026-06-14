#pragma once

#include <memory>

#include "vinter/color.hpp"

namespace vn {
    struct RendererSettings;
    class Window;

    class Renderer {
        friend class Engine;

    public:
        explicit Renderer(const RendererSettings& settings, const Window& window);
        ~Renderer();

    private:
        void begin_frame();
        void end_frame();

        Color m_clear_color {colors::DarkBlue};

        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // namespace vn