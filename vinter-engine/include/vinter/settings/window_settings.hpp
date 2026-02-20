#pragma once

#include <string>

namespace vn {
    struct WindowSettings {
        std::string title { "Vinter App" };

        struct Size {
            int width, height;
        };
        Size initial_size { 1280, 720 };
        Size virtual_size { 640,  360 };

        struct Flags {
            bool
            fullscreen          { false },
            resizeable          { false },
            maximized           { false },
            minimized           { false },
            hidden              { true  }, // To hide initial window black state until renderer construction.
            borderless          { false },
            always_on_top       { false },
            mouse_captured      { false },
            mouse_grabbed       { false },
            mouse_focus         { false },
            mouse_relative_mode { false },
            keyboard_grabbed    { false },
            high_pixel_density  { false };
        };
        Flags flags {};
    };
} // vn