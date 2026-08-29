#pragma once

#include <glm/glm.hpp>

#include "vinter/assets/handle.hpp"

struct SDL_Texture;

namespace vn {
    class Texture : public Handle {
        friend class TextureManager;
        using Handle::Handle;

    public:
        [[nodiscard]]
        std::size_t get_width() const noexcept;

        [[nodiscard]]
        std::size_t get_height() const noexcept;

        [[nodiscard]]
        glm::uvec2 get_size() const noexcept;

    private:
        Texture(Version version, Index index, glm::uvec2 size);

    private:
        glm::uvec2 m_size {};
    };
} // namespace vn