#pragma once

#include <cstdint>

#include "vinter/color.hpp"

namespace vn {
    struct RendererSettings {
        enum class Backend : std::uint8_t {
            Vulkan,
            Direct3D,
            Metal,
        };
        Backend backend {Backend::Vulkan};

        Color default_background_color {colors::DarkBlue};
    };
} // namespace vn