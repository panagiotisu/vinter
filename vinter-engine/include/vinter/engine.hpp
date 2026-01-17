#pragma once
#include <memory>

namespace vn {
    class Window;

    class Engine {
    public:
        Engine();
        virtual ~Engine();

        void run();

    protected:
        std::unique_ptr<Window> m_window;

        virtual void load() {}
        virtual void poll_events();
        virtual void update() {}
        virtual void render() {}

    private:
        bool m_running { false };
    };
} // vn