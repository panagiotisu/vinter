#pragma once

namespace vn {
    struct RendererSettings {
        enum class Backend {
            SDL,
            SDL_GPU,
            OpenGL,
        };
        Backend backend { Backend::SDL };

        enum class VSyncMode {
            Disabled,
            Enabled,
            Adaptive,
        };
        VSyncMode vsync_mode { VSyncMode::Disabled };
    };
} // vn