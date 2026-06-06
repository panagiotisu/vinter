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
#include "vinter/logger.hpp"
#include "vinter/renderer.hpp"
#include "vinter/settings/project_settings.hpp"
#include "vinter/time.hpp"
#include "vinter/window.hpp"

namespace vn {
    class Engine {
    public:
        explicit Engine(const ProjectSettings& project_settings);
        virtual ~Engine();

        void run();

    protected:
        virtual void load() {
        }

        virtual void poll_events() {
        }

        virtual void update(float delta) {
        }

        virtual void render() {
        }

        void quit();

        [[nodiscard]]
        auto get_window() noexcept -> const Window&;
        [[nodiscard]]
        auto get_renderer() noexcept -> const Renderer&;
        [[nodiscard]]
        auto get_time() noexcept -> const Time&;
        [[nodiscard]]
        auto get_devices() noexcept -> const DeviceManager&;
        [[nodiscard]]
        auto get_input() noexcept -> const InputMap&;

    private:
        std::unique_ptr<Window> m_window;
        std::unique_ptr<Renderer> m_renderer;
        std::unique_ptr<Time> m_time;
        std::unique_ptr<DeviceManager> m_devices;
        std::unique_ptr<InputMap> m_input;

        bool m_running {false};
    };
} // namespace vn