#pragma once

#include <memory>
#include <array>
#include <vector>
#include <unordered_map>
#include <optional>

union SDL_Event;

namespace vn {
    class Keyboard;
    class Mouse;
    class Gamepad;

    using DeviceID = std::uint32_t;

    class DeviceManager {
        friend class Engine;

    public:
        DeviceManager();

        static constexpr std::size_t MaxGamepadCount { 8 };

        [[nodiscard]] Keyboard& get_keyboard() const noexcept;
        [[nodiscard]] Mouse& get_mouse() const noexcept;
        [[nodiscard]] Gamepad* get_gamepad_by_id(DeviceID id) const noexcept;
        [[nodiscard]] Gamepad* get_gamepad(std::size_t slot) const noexcept;
        [[nodiscard]] std::array<Gamepad*, MaxGamepadCount> get_gamepads() const noexcept;
        [[nodiscard]] std::vector<Gamepad*> get_active_gamepads() const noexcept;

    private:
        void handle_events(const SDL_Event& event);
        void update();

        void handle_gamepad_added(DeviceID id);
        void handle_gamepad_removed(DeviceID id);

        std::unique_ptr<Keyboard> m_keyboard;
        std::unique_ptr<Mouse> m_mouse;
        std::array<std::optional<DeviceID>, MaxGamepadCount> m_gamepad_slots;
        std::unordered_map<DeviceID, std::unique_ptr<Gamepad>> m_gamepads;
    };
} // vn