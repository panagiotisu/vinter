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

        [[nodiscard]]
        auto vsync_enabled() const noexcept -> bool;
        void set_vsync(bool enabled);

    private:
        void begin_frame();
        void end_frame();

        Color m_clear_color {colors::DarkBlue};

        bool m_vsync_enabled {true};

        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // namespace vn