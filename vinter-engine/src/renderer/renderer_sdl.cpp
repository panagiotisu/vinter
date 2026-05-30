#include "renderer_sdl.hpp"

#include <numbers>

#include <SDL3/SDL.h>

#include "SDL3/SDL_render.h"
#include "vinter/assert.hpp"
#include "vinter/color.hpp"
#include "vinter/settings/renderer_settings.hpp"
#include "vinter/window.hpp"

namespace vn {
    struct RendererSDL::Impl {
        SDL_Renderer* sdl_renderer_backend {nullptr};

        Impl(const Impl&) = default;
        Impl(Impl&&) = delete;
        auto operator=(const Impl&) -> Impl& = default;
        auto operator=(Impl&&) -> Impl& = delete;

        Impl(const RendererSettings& renderer_settings, const Window& window)
            : sdl_renderer_backend(SDL_CreateRenderer(window.get_native_handle(), "")) {
            if (sdl_renderer_backend == nullptr) {
                throw std::runtime_error(SDL_GetError());
            }

            SDL_SetRenderVSync(
                sdl_renderer_backend, to_sdl_vsync_mode(renderer_settings.vsync_mode)
            );
        }

        ~Impl() {
            if (sdl_renderer_backend != nullptr) {
                SDL_DestroyRenderer(sdl_renderer_backend);
            }
        }

        static auto to_sdl_vsync_mode(const RendererSettings::VSyncMode vsync_mode) -> int {
            switch (vsync_mode) {
                case RendererSettings::VSyncMode::Disabled: return SDL_RENDERER_VSYNC_DISABLED;

                case RendererSettings::VSyncMode::Enabled: return 1;

                case RendererSettings::VSyncMode::Adaptive: return SDL_RENDERER_VSYNC_ADAPTIVE;
            }

            return SDL_RENDERER_VSYNC_DISABLED;
        }
    };

    RendererSDL::RendererSDL(const RendererSettings& renderer_settings, const Window& window)
        : m_impl(std::make_unique<Impl>(renderer_settings, window)) {
        // Show the window (briefly hidden on startup) AFTER Renderer has been
        // constructed, so that the window does not show blank state due to
        // non-existent renderer.
        SDL_ShowWindow(window.get_native_handle());
    }

    RendererSDL::~RendererSDL() = default;

    void RendererSDL::begin_frame() {
        const auto clear_color = get_clear_color();

        SDL_SetRenderDrawColorFloat(
            m_impl->sdl_renderer_backend,
            clear_color.r(),
            clear_color.g(),
            clear_color.b(),
            clear_color.a()
        );
        SDL_RenderClear(m_impl->sdl_renderer_backend);
    }

    void RendererSDL::end_frame() {
        SDL_RenderPresent(m_impl->sdl_renderer_backend);
    }

    void RendererSDL::draw_pixel(glm::vec2 position, Color color) {
        SDL_SetRenderDrawColorFloat(
            m_impl->sdl_renderer_backend, color.r(), color.g(), color.b(), color.a()
        );
        SDL_RenderPoint(m_impl->sdl_renderer_backend, position.x, position.y);
    }

    void RendererSDL::draw_line(glm::vec2 start, glm::vec2 end, Color color, float /*weight*/) {
        SDL_SetRenderDrawColorFloat(
            m_impl->sdl_renderer_backend, color.r(), color.g(), color.b(), color.a()
        );
        SDL_RenderLine(m_impl->sdl_renderer_backend, start.x, start.y, end.x, end.y);
    }

    void RendererSDL::draw_rectangle(glm::vec2 position, glm::vec2 size, Color color) {
        SDL_SetRenderDrawColorFloat(
            m_impl->sdl_renderer_backend, color.r(), color.g(), color.b(), color.a()
        );
        const SDL_FRect sdl_rectangle = {
            .x = position.x, .y = position.y, .w = size.x, .h = size.y
        };
        SDL_RenderFillRect(m_impl->sdl_renderer_backend, &sdl_rectangle);
    }

    void RendererSDL::draw_rectangle_line(
        glm::vec2 position,
        glm::vec2 size,
        Color color,
        float /*weight*/
    ) {
        SDL_SetRenderDrawColorFloat(
            m_impl->sdl_renderer_backend, color.r(), color.g(), color.b(), color.a()
        );
        const SDL_FRect sdl_rectangle = {
            .x = position.x, .y = position.y, .w = size.x, .h = size.y
        };
        SDL_RenderRect(m_impl->sdl_renderer_backend, &sdl_rectangle);
    }

    void RendererSDL::draw_polygon(std::vector<glm::vec2> vertices, Color color) {
    }

    void
    RendererSDL::draw_circle(glm::vec2 center, float radius, Color color, std::size_t segments) {
        VN_ASSERT(radius > 0, "Radius must be positive.");
        VN_ASSERT(segments > 2, "Need at least three segments to form a circle (closed chain).");

        std::vector<SDL_Vertex> vertices(segments + 2);

        // Construct center vertex.
        vertices[0] = {
            .position = {.x = center.x, .y = center.y},
            .color = {.r = color.r(), .g = color.g(), .b = color.b(), .a = color.a()}
        };

        // Construct perimeter vertices.
        for (std::size_t i = 0; i <= segments; ++i) {
            float theta = static_cast<float>(i) / static_cast<float>(segments) * 2.f
                          * std::numbers::pi_v<float>;

            vertices[i + 1] = {
                .position =
                    {.x = center.x + (std::cos(theta) * radius),
                     .y = center.y + (std::sin(theta) * radius)},
                .color = {.r = color.r(), .g = color.g(), .b = color.b(), .a = color.a()}
            };
        }

        std::vector<int> indices(segments * 3);

        for (int i = 0; i < segments; ++i) {
            indices[(i * 3) + 0] = 0;
            indices[(i * 3) + 1] = i + 1;
            indices[(i * 3) + 2] = i + 2;
        }

        SDL_RenderGeometry(
            m_impl->sdl_renderer_backend,
            nullptr,
            vertices.data(),
            static_cast<int>(vertices.size()),
            indices.data(),
            static_cast<int>(indices.size())
        );
    }

    void RendererSDL::draw_circle_line(glm::vec2 center, float radius, Color color, float weight) {
    }

} // namespace vn