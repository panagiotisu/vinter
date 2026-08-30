#pragma once

#include <filesystem>
#include <unordered_map>
#include <vector>

#include "vinter/graphics/texture.hpp"
#include "vinter/settings/texture_settings.hpp"

struct SDL_Texture;
struct SDL_Renderer;

namespace vn {
    class Renderer;

    class TextureManager {
    public:
        explicit TextureManager(const TextureSettings& settings);
        ~TextureManager() = default;

        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;

        TextureManager(TextureManager&&) = delete;
        TextureManager& operator=(TextureManager&&) = delete;

    public:
        [[nodiscard]]
        Texture load(const std::filesystem::path& path);
        [[nodiscard]]
        Texture
        load(const std::filesystem::path& path, TextureSettings::ScaleMode scale_mode_override);

        void unload(Texture texture);

        [[nodiscard]]
        SDL_Texture* try_get(Texture texture) const noexcept;

        [[nodiscard]]
        SDL_Texture* get(Texture texture) const;

        [[nodiscard]]
        bool contains(Texture texture) const noexcept;

        void attach_renderer(const Renderer& renderer);

        [[nodiscard]]
        static int to_native_scale_mode(TextureSettings::ScaleMode scale_mode);

    private:
        struct Slot {
            SDL_Texture* native_texture {};
            Texture::Version version {};
        };

    private:
        SDL_Renderer* m_renderer_native_handle {};

        TextureSettings::ScaleMode m_global_scale_mode { TextureSettings::ScaleMode::Nearest };
        std::vector<Slot> m_slots {};
        std::vector<Texture::Index> m_free_indices {};
        std::unordered_map<std::filesystem::path, Texture> m_loaded_textures {};
    };
} // namespace vn