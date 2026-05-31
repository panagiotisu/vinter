#pragma once

#include <algorithm>
#include <cstdint>

namespace vn
{
    struct ColorRGBA8
    {
        std::uint8_t r, g, b, a;
    };

    class Color
    {
    public:
        constexpr Color(float red, float green, float blue, float alpha = 1.f)
            : m_r(std::max(0.0f, red))
            , m_g(std::max(0.0f, green))
            , m_b(std::max(0.0f, blue))
            , m_a(std::max(0.0f, alpha))
        {
        }

        static constexpr auto from_hdr(float red, float green, float blue, float alpha = 1.f)
            -> Color
        {
            return {red, green, blue, alpha};
        }

        static constexpr auto
        from_rgba(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255) -> Color
        {
            return {
                static_cast<float>(r) / 255.0f,
                static_cast<float>(g) / 255.0f,
                static_cast<float>(b) / 255.0f,
                static_cast<float>(a) / 255.0f
            };
        }

        static constexpr auto from_rgba(ColorRGBA8 rgba) -> Color
        {
            return from_rgba(rgba.r, rgba.g, rgba.b, rgba.a);
        }

        [[nodiscard]]
        constexpr auto to_rgb_a8() const -> ColorRGBA8
        {
            return {
                .r = static_cast<std::uint8_t>(std::min(m_r, 1.0f) * 255.0f),
                .g = static_cast<std::uint8_t>(std::min(m_g, 1.0f) * 255.0f),
                .b = static_cast<std::uint8_t>(std::min(m_b, 1.0f) * 255.0f),
                .a = static_cast<std::uint8_t>(std::min(m_a, 1.0f) * 255.0f)
            };
        }

        [[nodiscard]]
        constexpr auto red() const -> float
        {
            return m_r;
        }

        [[nodiscard]]
        constexpr auto green() const -> float
        {
            return m_g;
        }

        [[nodiscard]]
        constexpr auto blue() const -> float
        {
            return m_b;
        }

        [[nodiscard]]
        constexpr auto alpha() const -> float
        {
            return m_a;
        }

    private:
        float m_r, m_g, m_b, m_a;
    };

    // NOLINTBEGIN(readability-identifier-naming)
    namespace colors
    {
        inline constexpr Color Red = Color::from_rgba(255, 0, 0);
        inline constexpr Color Green = Color::from_rgba(0, 255, 0);
        inline constexpr Color Blue = Color::from_rgba(0, 0, 255);
        inline constexpr Color Black = Color::from_rgba(0, 0, 0);
        inline constexpr Color White = Color::from_rgba(255, 255, 255);
        inline constexpr Color Blank = Color::from_rgba(0, 0, 0, 0);
        inline constexpr Color CornflowerBlue = Color::from_rgba(100, 149, 237);
        inline constexpr Color DarkBlue = Color::from_rgba(0, 82, 172);
        inline constexpr Color LightGray = Color::from_rgba(200, 200, 200);
        inline constexpr Color Gray = Color::from_rgba(130, 130, 130);
        inline constexpr Color DarkGray = Color::from_rgba(80, 80, 80);
        inline constexpr Color Yellow = Color::from_rgba(253, 249, 0);
        inline constexpr Color Gold = Color::from_rgba(255, 203, 0);
        inline constexpr Color Orange = Color::from_rgba(255, 161, 0);
        inline constexpr Color Pink = Color::from_rgba(255, 109, 194);
        inline constexpr Color Maroon = Color::from_rgba(190, 33, 55);
        inline constexpr Color Lime = Color::from_rgba(0, 158, 47);
        inline constexpr Color DarkGreen = Color::from_rgba(0, 117, 44);
        inline constexpr Color SkyBlue = Color::from_rgba(102, 191, 255);
        inline constexpr Color Purple = Color::from_rgba(200, 122, 255);
        inline constexpr Color Violet = Color::from_rgba(135, 60, 190);
        inline constexpr Color DarkPurple = Color::from_rgba(112, 31, 126);
        inline constexpr Color Beige = Color::from_rgba(211, 176, 131);
        inline constexpr Color Brown = Color::from_rgba(127, 106, 79);
        inline constexpr Color DarkBrown = Color::from_rgba(76, 63, 47);
        inline constexpr Color Magenta = Color::from_rgba(255, 0, 255);
        inline constexpr Color RayWhite = Color::from_rgba(245, 245, 245);
    } // namespace colors

    // NOLINTEND(readability-identifier-naming)

} // namespace vn