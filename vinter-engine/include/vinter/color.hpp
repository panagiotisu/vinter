#pragma once

#include <cstdint>

namespace vn {
    struct Color {
        std::uint8_t r, g, b, a;

        Color(
            const std::uint8_t red,
            const std::uint8_t green,
            const std::uint8_t blue,
            const std::uint8_t alpha = 255
        )
            : r(red), g(green), b(blue), a(alpha) {
        }
    };

    namespace colors {
        inline const Color RED             = {255, 0,   0  };
        inline const Color GREEN           = {0,   255, 0  };
        inline const Color BLUE            = {0,   0,   255};
        inline const Color BLACK           = {0,   0,   0  };
        inline const Color WHITE           = {255, 255, 255};
        inline const Color CORNFLOWER_BLUE = {100, 149, 237};
    } // colors
} //vn