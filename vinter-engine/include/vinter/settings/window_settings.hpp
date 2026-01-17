#pragma once

#include <string>

namespace vn {
    struct WindowSettings {
        std::string title { "Vinter App" };

        struct WindowSize {
            int width, height;
        };
        WindowSize initial_size { 1280, 720 };
        WindowSize virtual_size { 640,  360 };

        struct Flags {
            bool
            fullscreen,
            resizeable,
            maximized,
            minimized,
            hidden,
            borderless,
            on_top,
            mouse_captured,
            keyboard_grabbed,
            high_pixel_density;
        };
        Flags flags {};
    };
} // vn