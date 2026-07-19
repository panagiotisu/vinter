#pragma once

#include <memory>

// TODO: Place these in a fwd.hpp.
#include "vinter/color.hpp"
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
    class App {
    public:
        explicit App(const ProjectSettings& project_settings);
        virtual ~App();

        void run();

    protected:
        virtual void load() {
        }

        virtual void update() {
        }

        virtual void render() {
        }

        void quit();

        [[nodiscard]]
        auto get_window() noexcept -> Window&;
        [[nodiscard]]
        auto get_renderer() noexcept -> Renderer&;
        [[nodiscard]]
        auto get_time() noexcept -> Time&;
        [[nodiscard]]
        auto get_devices() noexcept -> DeviceManager&;
        [[nodiscard]]
        auto get_input() noexcept -> InputMap&;

    private:
        std::unique_ptr<Window> m_window {};
        std::unique_ptr<Renderer> m_renderer {};
        std::unique_ptr<Time> m_time {};
        std::unique_ptr<DeviceManager> m_devices {};
        std::unique_ptr<InputMap> m_input {};

        bool m_running { false };
    };
} // namespace vn