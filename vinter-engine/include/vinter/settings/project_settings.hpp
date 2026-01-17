#pragma once

#include "vinter/settings/window_settings.hpp"
#include "vinter/settings/renderer_settings.hpp"
#include "vinter/settings/physics_settings.hpp"

namespace vn {
    struct ProjectSettings {
        WindowSettings window_settings;
        RendererSettings renderer_settings;
        PhysicsSettings physics_settings;
    };
} // vn