#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "vinter/assets/handle.hpp"

struct SDL_Texture;

namespace vn {
    class Renderer;

    class Texture {
        friend class TextureManager;

    public:
        class Handle : public vn::Handle {
            friend class TextureManager;
            using vn::Handle::Handle;
        };

    private:
        Texture(glm::uvec2 size, SDL_Texture* handle) noexcept;

    public:
        Texture(Texture&& other) noexcept = default;

        Texture& operator=(Texture&& other) noexcept = default;

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        ~Texture();

    public:
        [[nodiscard]]
        std::size_t get_width() const noexcept;

        [[nodiscard]]
        std::size_t get_height() const noexcept;

        [[nodiscard]]
        glm::uvec2 get_size() const noexcept;

        [[nodiscard]]
        SDL_Texture* get_native_handle() const noexcept;

    private:
        glm::uvec2 m_size {};

        using TextureDeleter = void (*)(SDL_Texture*);
        std::unique_ptr<SDL_Texture, TextureDeleter> m_handle;
    };
} // namespace vn