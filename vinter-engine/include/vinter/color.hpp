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
        inline const Color RED             = { 255, 0,   0   };
        inline const Color GREEN           = { 0,   255, 0   };
        inline const Color BLUE            = { 0,   0,   255 };
        inline const Color BLACK           = { 0,   0,   0   };
        inline const Color WHITE           = { 255, 255, 255 };
        inline const Color CORNFLOWER_BLUE = { 100, 149, 237 };
        inline const Color DARK_BLUE       = { 0,   82,  172 };
        inline const Color LIGHT_GRAY      = { 200, 200, 200 };
        inline const Color GRAY            = { 130, 130, 130 };
        inline const Color DARK_GRAY       = { 80,  80,  80  };
        inline const Color YELLOW          = { 253, 249, 0   };
        inline const Color GOLD            = { 255, 203, 0   };
        inline const Color ORANGE          = { 255, 161, 0   };
        inline const Color PINK            = { 255, 109, 194 };
        inline const Color MAROON          = { 190, 33,  55  };
        inline const Color LIME            = { 0,   158, 47  };
        inline const Color DARK_GREEN      = { 0,   117, 44  };
        inline const Color SKYBLUE         = { 102, 191, 255 };
        inline const Color PURPLE          = { 200, 122, 255 };
        inline const Color VIOLET          = { 135, 60,  190 };
        inline const Color DARK_PURPLE     = { 112, 31,  126 };
        inline const Color BEIGE           = { 211, 176, 131 };
        inline const Color BROWN           = { 127, 106, 79  };
        inline const Color DARK_BROWN      = { 76,  63,  47  };
        inline const Color MAGENTA         = { 255, 0,   255 };
        inline const Color RAYWHITE        = { 245, 245, 245 };
        inline const Color BLANK           = { 0,   0,   0,    0 };
    } // colors
} //vn