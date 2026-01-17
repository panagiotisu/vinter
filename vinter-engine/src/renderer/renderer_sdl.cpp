#include "renderer_sdl.hpp"

#include <SDL3/SDL.h>

#include "vinter/window.hpp"
#include "vinter/settings/renderer_settings.hpp"

namespace vn {
    struct RendererSDL::Impl {
        SDL_Renderer* sdl_renderer_backend { nullptr };

        Impl(const RendererSettings &renderer_settings, const Window &window)
            : sdl_renderer_backend(SDL_CreateRenderer(window.get_native_handle(), "")) {
            if (!sdl_renderer_backend) throw std::runtime_error(SDL_GetError());

            SDL_SetRenderVSync(sdl_renderer_backend, to_sdl_vsync_mode(renderer_settings.vsync_mode));
        }

        ~Impl() {
            if (sdl_renderer_backend) SDL_DestroyRenderer(sdl_renderer_backend);
        }

        static int to_sdl_vsync_mode(const RendererSettings::VSyncMode vsync_mode) {
            switch (vsync_mode) {
                case RendererSettings::VSyncMode::Disabled:
                    return SDL_RENDERER_VSYNC_DISABLED;

                case RendererSettings::VSyncMode::Enabled:
                    return 1;

                case RendererSettings::VSyncMode::Adaptive:
                    return SDL_RENDERER_VSYNC_ADAPTIVE;
            }

            return SDL_RENDERER_VSYNC_DISABLED;
        }
    };

    RendererSDL::RendererSDL(const RendererSettings &renderer_settings, const Window &window)
        : m_impl(std::make_unique<Impl>(renderer_settings, window)) {
    }

    RendererSDL::~RendererSDL() = default;

    void RendererSDL::begin_frame() {
        SDL_RenderClear(m_impl->sdl_renderer_backend);
    }

    void RendererSDL::end_frame() {
        SDL_RenderPresent(m_impl->sdl_renderer_backend);
    }
} // vn