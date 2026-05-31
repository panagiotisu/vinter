#pragma once

#include <cstdint>

namespace vn
{
    struct RendererSettings
    {
        enum class Backend : std::uint8_t
        {
            Vulkan,
            Direct3D,
            Metal,
        };
        Backend backend {Backend::Vulkan};

        enum class VSyncMode : std::uint8_t
        {
            Disabled,
            Enabled,
            Adaptive,
        };
        VSyncMode vsync_mode {VSyncMode::Disabled};
    };
} // namespace vn