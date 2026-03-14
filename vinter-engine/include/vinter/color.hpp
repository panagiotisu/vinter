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
        inline const Color Red             = { 255, 0,   0   };
        inline const Color Green           = { 0,   255, 0   };
        inline const Color Blue            = { 0,   0,   255 };
        inline const Color Black           = { 0,   0,   0   };
        inline const Color White           = { 255, 255, 255 };
        inline const Color CornflowerBlue  = { 100, 149, 237 };
        inline const Color DarkBlue        = { 0,   82,  172 };
        inline const Color LightGray       = { 200, 200, 200 };
        inline const Color Gray            = { 130, 130, 130 };
        inline const Color DarkGray        = { 80,  80,  80  };
        inline const Color Yellow          = { 253, 249, 0   };
        inline const Color Gold            = { 255, 203, 0   };
        inline const Color Orange          = { 255, 161, 0   };
        inline const Color Pink            = { 255, 109, 194 };
        inline const Color Maroon          = { 190, 33,  55  };
        inline const Color Lime            = { 0,   158, 47  };
        inline const Color DarkGreen       = { 0,   117, 44  };
        inline const Color SkyBlue         = { 102, 191, 255 };
        inline const Color Purple          = { 200, 122, 255 };
        inline const Color Violet          = { 135, 60,  190 };
        inline const Color DarkPurple      = { 112, 31,  126 };
        inline const Color Beige           = { 211, 176, 131 };
        inline const Color Brown           = { 127, 106, 79  };
        inline const Color DarkBrown       = { 76,  63,  47  };
        inline const Color Magenta         = { 255, 0,   255 };
        inline const Color RayWhite        = { 245, 245, 245 };
        inline const Color Blank           = { 0,   0,   0, 0 };
    } // colors
} // vn