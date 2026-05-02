#pragma once

#include <cstdint>

namespace vn {
    struct Color {
        std::uint8_t r, g, b, a;

        constexpr Color(
            std::uint8_t red,
            std::uint8_t green,
            std::uint8_t blue,
            std::uint8_t alpha = 255
        )
            : r(red)
            , g(green)
            , b(blue)
            , a(alpha) {
        }

        [[nodiscard]]
        static constexpr auto Red() -> Color {
            return {255, 0, 0};
        }

        [[nodiscard]]
        static constexpr auto Green() -> Color {
            return {0, 255, 0};
        }

        [[nodiscard]]
        static constexpr auto Blue() -> Color {
            return {0, 0, 255};
        }

        [[nodiscard]]
        static constexpr auto Black() -> Color {
            return {0, 0, 0};
        }

        [[nodiscard]]
        static constexpr auto White() -> Color {
            return {255, 255, 255};
        }

        [[nodiscard]]
        static constexpr auto CornflowerBlue() -> Color {
            return {100, 149, 237};
        }

        [[nodiscard]]
        static constexpr auto DarkBlue() -> Color {
            return {0, 82, 172};
        }

        [[nodiscard]]
        static constexpr auto LightGray() -> Color {
            return {200, 200, 200};
        }

        [[nodiscard]]
        static constexpr auto Gray() -> Color {
            return {130, 130, 130};
        }

        [[nodiscard]]
        static constexpr auto DarkGray() -> Color {
            return {80, 80, 80};
        }

        [[nodiscard]]
        static constexpr auto Yellow() -> Color {
            return {253, 249, 0};
        }

        [[nodiscard]]
        static constexpr auto Gold() -> Color {
            return {255, 203, 0};
        }

        [[nodiscard]]
        static constexpr auto Orange() -> Color {
            return {255, 161, 0};
        }

        [[nodiscard]]
        static constexpr auto Pink() -> Color {
            return {255, 109, 194};
        }

        [[nodiscard]]
        static constexpr auto Maroon() -> Color {
            return {190, 33, 55};
        }

        [[nodiscard]]
        static constexpr auto Lime() -> Color {
            return {0, 158, 47};
        }

        [[nodiscard]]
        static constexpr auto DarkGreen() -> Color {
            return {0, 117, 44};
        }

        [[nodiscard]]
        static constexpr auto SkyBlue() -> Color {
            return {102, 191, 255};
        }

        [[nodiscard]]
        static constexpr auto Purple() -> Color {
            return {200, 122, 255};
        }

        [[nodiscard]]
        static constexpr auto Violet() -> Color {
            return {135, 60, 190};
        }

        [[nodiscard]]
        static constexpr auto DarkPurple() -> Color {
            return {112, 31, 126};
        }

        [[nodiscard]]
        static constexpr auto Beige() -> Color {
            return {211, 176, 131};
        }

        [[nodiscard]]
        static constexpr auto Brown() -> Color {
            return {127, 106, 79};
        }

        [[nodiscard]]
        static constexpr auto DarkBrown() -> Color {
            return {76, 63, 47};
        }

        [[nodiscard]]
        static constexpr auto Magenta() -> Color {
            return {255, 0, 255};
        }

        [[nodiscard]]
        static constexpr auto RayWhite() -> Color {
            return {245, 245, 245};
        }

        [[nodiscard]]
        static constexpr auto Blank() -> Color {
            return {0, 0, 0, 0};
        }
    };
} // namespace vn