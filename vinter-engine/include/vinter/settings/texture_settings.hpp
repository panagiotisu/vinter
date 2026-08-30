#pragma once

#include <cstdint>

namespace vn {
    struct TextureSettings {
        enum class ScaleMode : std::uint8_t {
            Nearest,  // Nearest pixel sampling.
            Linear,   // Linear filtering.
            PixelArt, // Nearest pixel sampling with improved scaling for pixel art.
        };
        ScaleMode global_scale_mode { ScaleMode::Nearest };
    };
} // namespace vn