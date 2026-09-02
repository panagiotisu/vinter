#include "vinter/assets/font_manager.hpp"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "vinter/logger.hpp"

namespace vn {
    FontManager::~FontManager() {
        for (auto slot : m_slots) {
            if (slot.native_font != nullptr) {
                TTF_CloseFont(slot.native_font);
                slot.native_font = nullptr;
            }
        }
    }

    Font FontManager::load(const std::filesystem::path& path, std::size_t size) {
        const auto normalized_path = std::filesystem::canonical(path);
        const std::string key = normalized_path.string() + "|" + std::to_string(size);

        if (const auto it = m_loaded_fonts.find(key); it != m_loaded_fonts.end()) {
            return it->second;
        }

        TTF_Font* native_font = TTF_OpenFont(normalized_path.c_str(), static_cast<float>(size));
        VN_ASSERT(
            native_font != nullptr,
            "Failed to load font '{}': {}",
            normalized_path.string(),
            SDL_GetError()
        );

        Font::Index index {};
        if (!m_free_indices.empty()) {
            index = m_free_indices.back();
            m_free_indices.pop_back();
        } else {
            index = static_cast<Font::Index>(m_slots.size());
            m_slots.emplace_back(Slot {});
        }

        auto& slot = m_slots[index];
        if (slot.version == 0) {
            slot.version = 1;
        }
        slot.native_font = native_font;

        Font font(slot.version, index);
        m_loaded_fonts.emplace(key, font);
        return font;
    }

    void FontManager::unload(Font font) {
        if (font.is_null()) {
            return;
        }

        const auto index = font.index();
        if (index >= m_slots.size()) {
            return;
        }

        auto& slot = m_slots[index];
        if (slot.version != font.version()) {
            return; // stale
        }

        if (slot.native_font != nullptr) {
            TTF_CloseFont(slot.native_font);
            slot.native_font = nullptr;
        }

        ++slot.version;
        if (slot.version > Font::MaxVersion) {
            slot.version = 1;
        }
        m_free_indices.push_back(index);

        // Remove from cache map
        for (auto it = m_loaded_fonts.begin(); it != m_loaded_fonts.end(); ++it) {
            if (it->second == font) {
                m_loaded_fonts.erase(it);
                break;
            }
        }
    }

    TTF_Font* FontManager::try_get(Font font) const noexcept {
        if (font.is_null()) {
            return nullptr;
        }

        const auto index = font.index();
        if (index >= m_slots.size()) {
            return nullptr;
        }

        const auto& slot = m_slots[index];
        if (slot.version != font.version()) {
            return nullptr;
        }

        return slot.native_font;
    }

    TTF_Font* FontManager::get(Font font) const {
        auto* native_font = try_get(font);
        VN_ASSERT(native_font != nullptr, "Invalid or stale Font: {}", font.id());
        return native_font;
    }

    bool FontManager::contains(Font font) const noexcept {
        return try_get(font) != nullptr;
    }
} // namespace vn