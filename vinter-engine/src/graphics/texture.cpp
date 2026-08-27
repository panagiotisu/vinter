#include "vinter/graphics/texture.hpp"

#include <SDL3/SDL.h>

namespace vn {
    Texture::Texture(glm::uvec2 size, SDL_Texture* handle) noexcept
        : m_size(size)
        , m_handle(handle, SDL_DestroyTexture) {
    }

    std::size_t Texture::get_width() const noexcept {
        return m_size.x;
    }

    std::size_t Texture::get_height() const noexcept {
        return m_size.y;
    }

    glm::uvec2 Texture::get_size() const noexcept {
        return m_size;
    }
} // namespace vn