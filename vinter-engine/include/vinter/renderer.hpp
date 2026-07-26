#pragma once

#include <string>

#include "vinter/color.hpp"
#include "vinter/settings/renderer_settings.hpp"

struct SDL_Renderer;
struct SDL_Window;

namespace vn {
    class Window;

    class Renderer {
        friend class App;

    public:
        Renderer(const RendererSettings& settings, const Window& window);
        ~Renderer();

        void set_clear_color(Color color);

        void set_vsync(RendererSettings::VSyncMode vsync);

    private:
        void begin_frame();
        void end_frame();

        void clear();

    private:
        Color m_clear_color { colors::DarkBlue };

        SDL_Renderer* m_renderer_backend {};
    };
} // namespace vn