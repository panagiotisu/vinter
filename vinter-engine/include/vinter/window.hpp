#pragma once

#include <memory>

namespace vn {
    struct WindowSettings;

    class Window {
    public:
        explicit Window(const WindowSettings& window_settings);
        ~Window();

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // vn