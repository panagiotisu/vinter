#pragma once

#include <memory>

// TODO: Place these in a fwd.hpp.
#include "vinter/color.hpp"
#include "vinter/game_object.hpp"
#include "vinter/input/device_manager.hpp"
#include "vinter/input/gamepad.hpp"
#include "vinter/input/input_map.hpp"
#include "vinter/input/keyboard.hpp"
#include "vinter/input/mouse.hpp"
#include "vinter/renderer.hpp"
#include "vinter/settings/project_settings.hpp"
#include "vinter/time.hpp"
#include "vinter/window.hpp"

namespace vn {
    class Engine {
    public:
        explicit Engine(const ProjectSettings& projectSettings);
        virtual ~Engine();

        void Run();

    protected:
        std::unique_ptr<Window> m_window;
        std::unique_ptr<Renderer> m_renderer;
        std::unique_ptr<Time> m_time;
        std::unique_ptr<DeviceManager> m_devices;
        std::unique_ptr<InputMap> m_input;

        virtual void Load() {
        }

        virtual void PollEvents() {
        }

        virtual void Update(float delta) {
        }

        virtual void Render() {
        }

        void Quit();

    private:
        bool m_running {false};
    };
} // namespace vn