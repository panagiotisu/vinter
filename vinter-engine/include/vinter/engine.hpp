#pragma once

#include <memory>

// TODO: Place these in a fwd.hpp.
#include "vinter/settings/project_settings.hpp"
#include "vinter/window.hpp"
#include "vinter/color.hpp"
#include "vinter/renderer.hpp"
#include "vinter/time.hpp"
#include "vinter/input/keyboard.hpp"
#include "vinter/input/mouse.hpp"
#include "vinter/input/gamepad.hpp"
#include "vinter/input/device_manager.hpp"
#include "vinter/input/input_map.hpp"
// #include "vinter/component.hpp"
// #include "vinter/game_object.hpp"

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
        std::unique_ptr<DeviceManager> devices;
        std::unique_ptr<InputMap> input;

        virtual void load() {}
        virtual void poll_events() {}
        virtual void update(float delta) {}
        virtual void render() {}

    private:
        bool m_running { false };
    };
} // vn