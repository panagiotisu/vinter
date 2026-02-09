#pragma once

#include <memory>
#include <unordered_map>

namespace vn {
    class Keyboard;
    class Mouse;
    class Joystick;
    class Gamepad;

    class DeviceManager {
        friend class Engine;
    public:
        DeviceManager();
        ~DeviceManager();

    private:
        std::unique_ptr<Keyboard> m_keyboard;
        std::unique_ptr<Mouse> m_mouse;
        std::unordered_map<unsigned int, std::unique_ptr<Joystick>> m_joysticks;
        std::unordered_map <unsigned int, std::unique_ptr<Gamepad>> m_gamepads;
    };
} // vn