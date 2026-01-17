#pragma once

#include "vinter/settings/window_settings.hpp"
#include "vinter/settings/renderer_settings.hpp"
#include "vinter/settings/physics_settings.hpp"

namespace vn {
    struct ProjectSettings {
        WindowSettings window;
        RendererSettings renderer;
        PhysicsSettings physics;
    };
} // vn