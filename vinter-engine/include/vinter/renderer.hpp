#pragma once

#include <memory>

#include "vinter/color.hpp"

namespace vn {
    struct RendererSettings;
    class Window;

    class Renderer {
        friend class Engine;

    public:
        static std::unique_ptr<Renderer> create(const RendererSettings& renderer_settings, const Window& window);
        virtual ~Renderer() = 0;

        void set_clear_color(Color color);

    protected:
        [[nodiscard]] Color get_clear_color() const;

    private:
        Color m_clear_color { colors::BLACK };

        virtual void begin_frame() = 0;
        virtual void end_frame() = 0;
    };
} // vn