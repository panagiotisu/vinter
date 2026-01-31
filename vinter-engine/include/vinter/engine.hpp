#pragma once

#include <memory>

// TODO: Place these in a fwd.hpp.
#include "vinter/settings/project_settings.hpp"
#include "vinter/window.hpp"
#include "vinter/color.hpp"
#include "vinter/renderer.hpp"
#include "vinter/time.hpp"
#include "input/keyboard.hpp"
#include "input/mouse.hpp"
#include "input/input_map.hpp"

namespace vn {
    class Engine {
    public:
        explicit Engine(const ProjectSettings& project_settings);
        virtual ~Engine();

        void run();

    protected:
        std::unique_ptr<Window> window;
        std::unique_ptr<Renderer> renderer;
        std::unique_ptr<Time> time;
        std::unique_ptr<Keyboard> keyboard;
        std::unique_ptr<Mouse> mouse;
        std::unique_ptr<InputMap> input;

        virtual void load() {}
        virtual void poll_events() {}
        virtual void update(float delta) {}
        virtual void render() {}

    private:
        bool m_running { false };
    };
} // vn