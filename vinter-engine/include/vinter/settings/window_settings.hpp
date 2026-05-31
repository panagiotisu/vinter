#pragma once

#include <string>

namespace vn
{
    struct WindowSettings
    {
        std::string title {"Vinter App"};

        struct Size
        {
            int width, height;
        };

        Size initial_size {.width = 1280, .height = 720};
        Size virtual_size {.width = 640, .height = 360};

        struct Flags
        {
            bool fullscreen {false};
            bool resizeable {false};
            bool maximized {false};
            bool minimized {false};
            bool hidden {true}; // To hide initial window black state until renderer construction.
            bool borderless {false};
            bool always_on_top {false};
            bool mouse_captured {false};
            bool mouse_grabbed {false};
            bool mouse_focus {false};
            bool mouse_relative_mode {false};
            bool keyboard_grabbed {false};
            bool high_pixel_density {false};
        };

        Flags flags {};
    };
} // namespace vn