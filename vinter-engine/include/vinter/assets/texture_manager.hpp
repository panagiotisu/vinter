#pragma once

#include <filesystem>
#include <optional>
#include <unordered_map>
#include <vector>

#include "vinter/graphics/texture.hpp"

namespace vn {
    class Renderer;

    class TextureManager {
    public:
        explicit TextureManager(const Renderer& renderer);

        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;

        TextureManager(TextureManager&&) = delete;
        TextureManager& operator=(TextureManager&&) = delete;

        ~TextureManager() = default;

    public:
        [[nodiscard]]
        Texture::Handle load(const std::filesystem::path& path);

        void unload(Texture::Handle texture);

        [[nodiscard]]
        const Texture* try_get(Texture::Handle texture) const noexcept;

        [[nodiscard]]
        const Texture& get(Texture::Handle texture) const;

        [[nodiscard]]
        bool contains(Texture::Handle texture) const noexcept;

    private:
        struct Slot {
            std::optional<Texture> texture;
            Texture::Handle::Version version {};
        };

        [[nodiscard]]
        Texture create_texture(const std::filesystem::path& path) const;

    private:
        const Renderer& m_renderer;

        std::vector<Slot> m_slots;
        std::vector<Texture::Handle::Index> m_free_indices {};
        std::unordered_map<std::filesystem::path, Texture::Handle> m_loaded_textures {};
    };
} // namespace vn