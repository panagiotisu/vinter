#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

union SDL_Event;

namespace vn {
    class Keyboard;
    class Mouse;
    class Gamepad;

    class DeviceManager {
        friend class Engine;

    public:
        DeviceManager();

        [[nodiscard]] Keyboard& get_keyboard() const noexcept;
        [[nodiscard]] Mouse& get_mouse() const noexcept;
        [[nodiscard]] std::vector<Gamepad*> get_gamepads() const noexcept;
        [[nodiscard]] Gamepad* get_gamepad(unsigned int gamepad_id) const noexcept;

    private:
        void handle_events(const SDL_Event& event);
        void update();

        void handle_gamepad_added(unsigned int gamepad_id);

        std::unique_ptr<Keyboard> m_keyboard;
        std::unique_ptr<Mouse> m_mouse;
        std::unordered_map<unsigned int, std::unique_ptr<Gamepad>> m_gamepads;
    };
} // vn