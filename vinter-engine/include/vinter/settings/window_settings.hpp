#pragma once

#include <cstdint>
#include <string>

#include <glm/glm.hpp>

namespace vn {
    struct WindowSettings {
        std::string title { "Vinter App" };

        glm::uvec2 initial_size { 1280, 720 };
        glm::uvec2 virtual_size { 640, 360 };

        struct Flags {
            bool fullscreen { false };
            bool resizeable { false };
            bool maximized { false };
            bool minimized { false };
            bool hidden { true }; // To hide initial window black state until renderer construction.
            bool borderless { false };
            bool always_on_top { false };
            bool mouse_captured { false };
            bool mouse_grabbed { false };
            bool mouse_focus { false };
            bool mouse_relative_mode { false };
            bool keyboard_grabbed { false };
            bool high_pixel_density { false };
        };

        Flags flags {};
    };
} // namespace vn