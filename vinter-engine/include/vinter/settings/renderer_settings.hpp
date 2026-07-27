#pragma once

#include <cstdint>

#include "vinter/color.hpp"

namespace vn {
    struct RendererSettings {
        enum class Backend : std::uint8_t {
            Vulkan,
            Direct3D12,
            Metal,
            Automatic,
        };
        Backend backend { Backend::Automatic };

        enum class VSyncMode : std::int8_t {
            Adaptive = -1,
            Disabled = 0,
            Enabled = 1,
        };
        VSyncMode vsync { VSyncMode::Adaptive };

        Color default_clear_color { colors::DarkBlue };
    };
} // namespace vn