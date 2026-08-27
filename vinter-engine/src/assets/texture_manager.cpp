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

    Texture::Handle TextureManager::load(const std::filesystem::path& path) {
        const auto normalized_path = std::filesystem::canonical(path);

        if (const auto it = m_loaded_textures.find(normalized_path);
            it != m_loaded_textures.end()) {
            return it->second;
        }

        Texture::Handle::Index index {};

        if (!m_free_indices.empty()) {
            index = m_free_indices.back();
            m_free_indices.pop_back();
        } else {
            index = static_cast<Handle::Index>(m_slots.size());
            m_slots.emplace_back(Slot {});
        }

        auto& slot = m_slots[index];

        // A new slot starts at generation 1.
        // Reused slots increment their generation during unload().
        if (slot.version == 0) {
            slot.version = 1;
        }

        slot.texture.emplace(create_texture(normalized_path));

        const Texture::Handle texture { slot.version, index };
        m_loaded_textures.emplace(normalized_path, texture);

        return texture;
    }

    void TextureManager::unload(Texture::Handle texture) {
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

        if (!slot.texture.has_value()) {
            return;
        }

        // Remove the texture handle from the map.
        for (auto it = m_loaded_textures.begin(); it != m_loaded_textures.end(); ++it) {
            if (it->second == texture) {
                m_loaded_textures.erase(it);
                break;
            }
        }

        slot.texture.reset();

        ++slot.version;

        if (slot.version > Texture::Handle::MaxVersion) {
            slot.version = 1;
        }

        m_free_indices.push_back(index);
    }

    const Texture* TextureManager::try_get(Texture::Handle texture) const noexcept {
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

        if (!slot.texture.has_value()) {
            return nullptr;
        }

        return &slot.texture.value();
    }

    const Texture& TextureManager::get(Texture::Handle texture) const {
        const Texture* texture_ptr = try_get(texture);
        VN_ASSERT(texture_ptr != nullptr, "Invalid or stale Texture::Handle: {}", texture.id());
        return *texture_ptr;
    }

    bool TextureManager::contains(Texture::Handle texture) const noexcept {
        return try_get(texture) != nullptr;
    }

    Texture TextureManager::create_texture(const std::filesystem::path& path) const {
        VN_ASSERT(
            path.extension() == ".png",
            "Unsupported texture image format:\n{}\n"
            "Only PNGs are currently supported.",
            path.string()
        );

        SDL_Surface* surface = SDL_LoadPNG(path.c_str());
        VN_ASSERT(surface != nullptr, "Failed to load PNG '{}': {}", path.string(), SDL_GetError());
        const glm::uvec2 size { surface->w, surface->h };

        SDL_Texture* native_handle = SDL_CreateTextureFromSurface(
            m_renderer.get_native_handle(), surface
        );
        VN_ASSERT(
            native_handle != nullptr,
            "Failed to create texture '{}': {}",
            path.string(),
            SDL_GetError()
        );

        return Texture { size, native_handle };
    }
} // namespace vn