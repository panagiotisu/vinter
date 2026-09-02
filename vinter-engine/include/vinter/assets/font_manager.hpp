#pragma once

#include <filesystem>
#include <unordered_map>
#include <vector>

#include "vinter/graphics/font.hpp"

struct TTF_Font;

namespace vn {
    class FontManager {
    public:
        FontManager() = default;
        ~FontManager();

        FontManager(const FontManager&) = delete;
        FontManager& operator=(const FontManager&) = delete;

        FontManager(FontManager&&) = delete;
        FontManager& operator=(FontManager&&) = delete;

    public:
        Font load(const std::filesystem::path& path, std::size_t size);

        void unload(Font font);

        [[nodiscard]]
        TTF_Font* try_get(Font font) const noexcept;

        [[nodiscard]]
        TTF_Font* get(Font font) const;

        [[nodiscard]]
        bool contains(Font font) const noexcept;

    private:
        struct Slot {
            TTF_Font* native_font {};
            Font::Version version {};
        };

        std::unordered_map<std::string, Font> m_loaded_fonts;
        std::vector<Slot> m_slots;
        std::vector<Font::Index> m_free_indices;
    };
} // namespace vn