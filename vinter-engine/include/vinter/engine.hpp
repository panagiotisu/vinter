#pragma once

namespace vn {
    class Engine {
    public:
        Engine();
        virtual ~Engine();

        void run();

    protected:
        virtual void load() {}
        virtual void poll_events() {}
        virtual void update() {}
        virtual void render() {}

    private:
        bool m_running { false };
    };
} // vn