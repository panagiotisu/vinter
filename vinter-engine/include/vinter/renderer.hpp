#pragma once

#include <memory>

namespace vn {
    struct RendererSettings;
    class Window;

    class Renderer {
        friend class Engine;

    public:
        explicit Renderer(const RendererSettings& settings);
        ~Renderer();

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // namespace vn