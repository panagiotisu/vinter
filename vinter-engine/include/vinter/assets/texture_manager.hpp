#pragma once

#include <filesystem>
#include <optional>
#include <unordered_map>
#include <vector>

#include "vinter/graphics/texture.hpp"

struct SDL_Texture;

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
        Texture load(const std::filesystem::path& path);

        void unload(Texture texture);

        [[nodiscard]]
        SDL_Texture* try_get(Texture texture) const noexcept;

        [[nodiscard]]
        SDL_Texture* get(Texture texture) const;

        [[nodiscard]]
        bool contains(Texture texture) const noexcept;

    private:
        struct Slot {
            SDL_Texture* native_texture {};
            Texture::Version version {};
        };

    private:
        const Renderer& m_renderer;

        std::vector<Slot> m_slots;
        std::vector<Texture::Index> m_free_indices {};
        std::unordered_map<std::filesystem::path, Texture> m_loaded_textures {};
    };
} // namespace vn