#pragma once
#include <memory>

namespace vn {
    struct ProjectSettings;
    class Window;
    class Renderer;

    class Engine {
    public:
        explicit Engine(const ProjectSettings& project_settings);
        virtual ~Engine();

        void run();

    protected:
        std::unique_ptr<Window> window;
        std::unique_ptr<Renderer> renderer;

        virtual void load() {}
        virtual void poll_events();
        virtual void update() {}
        virtual void render() {}

    private:
        bool m_running { false };
    };
} // vn