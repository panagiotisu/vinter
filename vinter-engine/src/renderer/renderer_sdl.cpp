#include "renderer_sdl.hpp"

#include <algorithm>
#include <cmath>

#include <SDL3/SDL.h>

#include "SDL3/SDL_render.h"
#include "glm/ext/scalar_constants.hpp"
#include "vinter/assert.hpp"
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
        // Show the window (briefly hidden on startup) AFTER Renderer has been
        // constructed, so that the window does not show blank state due to
        // non-existent renderer.
        SDL_ShowWindow(window.GetNativeHandle());
    }

    RendererSDL::~RendererSDL() = default;

    void RendererSDL::BeginFrame() {
        const auto clearColor = GetClearColor();

        SDL_SetRenderDrawColorFloat(
            m_impl->sdlRendererBackend,
            clearColor.R(),
            clearColor.G(),
            clearColor.B(),
            clearColor.A()
        );
        SDL_RenderClear(m_impl->sdlRendererBackend);
    }

    void RendererSDL::EndFrame() {
        SDL_RenderPresent(m_impl->sdlRendererBackend);
    }

    void RendererSDL::DrawPixel(glm::vec2 position, Color color) {
        SDL_SetRenderDrawColorFloat(
            m_impl->sdlRendererBackend, color.R(), color.G(), color.B(), color.A()
        );
        SDL_RenderPoint(m_impl->sdlRendererBackend, position.x, position.y);
    }

    void RendererSDL::DrawLine(glm::vec2 start, glm::vec2 end, Color color, float /*weight*/) {
        SDL_SetRenderDrawColorFloat(
            m_impl->sdlRendererBackend, color.R(), color.G(), color.B(), color.A()
        );
        SDL_RenderLine(m_impl->sdlRendererBackend, start.x, start.y, end.x, end.y);
    }

    void RendererSDL::DrawRectangle(glm::vec2 position, glm::vec2 size, Color color) {
        SDL_SetRenderDrawColorFloat(
            m_impl->sdlRendererBackend, color.R(), color.G(), color.B(), color.A()
        );
        const SDL_FRect sdlRectangle = {.x = position.x, .y = position.y, .w = size.x, .h = size.y};
        SDL_RenderFillRect(m_impl->sdlRendererBackend, &sdlRectangle);
    }

    void RendererSDL::DrawRectangleLine(
        glm::vec2 position,
        glm::vec2 size,
        Color color,
        float /*weight*/
    ) {
        SDL_SetRenderDrawColorFloat(
            m_impl->sdlRendererBackend, color.R(), color.G(), color.B(), color.A()
        );
        const SDL_FRect sdlRectangle = {.x = position.x, .y = position.y, .w = size.x, .h = size.y};
        SDL_RenderRect(m_impl->sdlRendererBackend, &sdlRectangle);
    }

    void RendererSDL::DrawPolygon(std::vector<glm::vec2> vertices, Color color) {
    }

    void
    RendererSDL::DrawCircle(glm::vec2 center, float radius, Color color, std::size_t segments) {
        VN_ASSERT(radius > 0, "Radius must be positive.");
        VN_ASSERT(segments > 2, "Need at least three segments to form a circle (closed chain).");
    }

    void RendererSDL::DrawCircleLine(glm::vec2 center, float radius, Color color, float weight) {
    }

} // namespace vn