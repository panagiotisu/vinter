#include "renderer_sdl.hpp"

#include <SDL3/SDL.h>

#include "vinter/color.hpp"
#include "vinter/settings/renderer_settings.hpp"
#include "vinter/window.hpp"

namespace vn {
    struct RendererSDL::Impl {
        SDL_Renderer* sdlRendererBackend {nullptr};

        Impl(const Impl&) = default;
        Impl(Impl&&) = delete;
        auto operator=(const Impl&) -> Impl& = default;
        auto operator=(Impl&&) -> Impl& = delete;

        Impl(const RendererSettings& rendererSettings, const Window& window)
            : sdlRendererBackend(SDL_CreateRenderer(window.GetNativeHandle(), "")) {
            if (sdlRendererBackend == nullptr) {
                throw std::runtime_error(SDL_GetError());
            }

            SDL_SetRenderVSync(sdlRendererBackend, ToSdlVsyncMode(rendererSettings.vsync_mode));
        }

        ~Impl() {
            if (sdlRendererBackend != nullptr) {
                SDL_DestroyRenderer(sdlRendererBackend);
            }
        }

        static auto ToSdlVsyncMode(const RendererSettings::VSyncMode vsyncMode) -> int {
            switch (vsyncMode) {
                case RendererSettings::VSyncMode::Disabled: return SDL_RENDERER_VSYNC_DISABLED;

                case RendererSettings::VSyncMode::Enabled: return 1;

                case RendererSettings::VSyncMode::Adaptive: return SDL_RENDERER_VSYNC_ADAPTIVE;
            }

            return SDL_RENDERER_VSYNC_DISABLED;
        }
    };

    RendererSDL::RendererSDL(const RendererSettings& rendererSettings, const Window& window)
        : m_impl(std::make_unique<Impl>(rendererSettings, window)) {
        // Show the window (briefly hidden on startup) AFTER Renderer has been constructed, so that
        // the window does not show blank state due to non-existent renderer.
        SDL_ShowWindow(window.GetNativeHandle());
    }

    RendererSDL::~RendererSDL() = default;

    void RendererSDL::BeginFrame() {
        const auto clearColor = GetClearColor();

        SDL_SetRenderDrawColor(
            m_impl->sdlRendererBackend, clearColor.r, clearColor.g, clearColor.b, clearColor.a
        );
        SDL_RenderClear(m_impl->sdlRendererBackend);
    }

    void RendererSDL::EndFrame() {
        SDL_RenderPresent(m_impl->sdlRendererBackend);
    }
} // namespace vn