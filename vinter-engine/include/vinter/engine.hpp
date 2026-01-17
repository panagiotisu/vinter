#pragma once
#include <memory>

namespace vn {
    struct ProjectSettings;
    class Window;

    class Engine {
    public:
        explicit Engine(const ProjectSettings& project_settings);
        virtual ~Engine();

        void run();

    protected:
        std::unique_ptr<Window> window;

        virtual void load() {}
        virtual void poll_events();
        virtual void update() {}
        virtual void render() {}

    private:
        bool m_running { false };
    };
} // vn