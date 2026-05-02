#pragma once

#include <cstdint>

namespace vn {
    struct RendererSettings {
        enum class Backend : std::uint8_t {
            SDL,
            SdlGpu,
            OpenGL,
        };
        Backend backend {Backend::SDL};

        enum class VSyncMode : std::uint8_t {
            Disabled,
            Enabled,
            Adaptive,
        };
        VSyncMode vsync_mode {VSyncMode::Disabled};
    };
} // namespace vn