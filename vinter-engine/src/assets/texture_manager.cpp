#include "vinter/assets/texture_manager.hpp"

#include <SDL3/SDL.h>

#include "vinter/graphics/renderer.hpp"
#include "vinter/logger.hpp"

namespace vn {
    TextureManager::TextureManager(const Renderer& renderer) : m_renderer(renderer) {
        VN_ASSERT(
            m_renderer.get_native_handle() != nullptr, "TextureManager requires a valid renderer."
        );
    }

    Texture TextureManager::load(const std::filesystem::path& path) {
        const auto normalized_path = std::filesystem::canonical(path);

        if (const auto it = m_loaded_textures.find(normalized_path);
            it != m_loaded_textures.end()) {
            return it->second;
        }

        SDL_Surface* surface = SDL_LoadPNG(normalized_path.c_str());
        VN_ASSERT(
            surface != nullptr,
            "Failed to load PNG '{}': {}",
            normalized_path.string(),
            SDL_GetError()
        );

        SDL_Texture* native = SDL_CreateTextureFromSurface(m_renderer.get_native_handle(), surface);
        SDL_DestroySurface(surface);
        VN_ASSERT(
            native != nullptr,
            "Failed to create texture '{}': {}",
            normalized_path.string(),
            SDL_GetError()
        );

        const glm::uvec2 size = { surface->w, surface->h };
        Texture::Index index {};

        if (!m_free_indices.empty()) {
            index = m_free_indices.back();
            m_free_indices.pop_back();
        } else {
            index = static_cast<Texture::Index>(m_slots.size());
            m_slots.emplace_back(Slot {});
        }

        auto& slot = m_slots[index];

        // A new slot starts at generation 1.
        // Reused slots increment their generation during unload().
        if (slot.version == 0) {
            slot.version = 1;
        }

        slot.native_texture = native;

        Texture texture(slot.version, index, size);
        m_loaded_textures.emplace(normalized_path, texture);

        return texture;
    }

    void TextureManager::unload(Texture texture) {
        if (texture.is_null()) {
            return;
        }

        const auto index = texture.index();

        if (index >= m_slots.size()) {
            return;
        }

        auto& slot = m_slots[index];

        // Check for stale texture.
        if (slot.version != texture.version()) {
            return;
        }

        if (slot.native_texture != nullptr) {
            return;
        }

        SDL_DestroyTexture(slot.native_texture);
        slot.native_texture = nullptr;

        ++slot.version;

        if (slot.version > Texture::MaxVersion) {
            slot.version = 1;
        }

        m_free_indices.push_back(index);
    }

    SDL_Texture* TextureManager::try_get(Texture texture) const noexcept {
        if (texture.is_null()) {
            return nullptr;
        }

        const auto index = texture.index();
        if (index >= m_slots.size()) {
            return nullptr;
        }

        const auto& slot = m_slots[index];
        if (slot.version != texture.version()) {
            return nullptr;
        }

        return slot.native_texture;
    }

    SDL_Texture* TextureManager::get(Texture texture) const {
        auto* native_texture = try_get(texture);
        VN_ASSERT(native_texture != nullptr, "Invalid or stale Texture: {}", texture.id());
        return native_texture;
    }

    bool TextureManager::contains(Texture texture) const noexcept {
        return try_get(texture) != nullptr;
    }
} // namespace vn