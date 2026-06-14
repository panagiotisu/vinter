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
        Backend backend {Backend::Automatic};

        Color default_background_color {colors::DarkBlue};
    };
} // namespace vn