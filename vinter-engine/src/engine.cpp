#include "vinter/engine.hpp"

namespace vn {
    Engine::Engine() {}

    Engine::~Engine() {}

    void Engine::run() {
        m_running = true;

        load();
        while (m_running) {
            poll_events();
            update();
            render();
        }
    }
} // vn