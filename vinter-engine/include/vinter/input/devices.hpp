#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

union SDL_Event;

namespace vn {
    class Keyboard;
    class Mouse;
    class Gamepad;

    using DeviceID = std::uint32_t;

    class Devices {
        friend class App;

    public:
        Devices();

        static constexpr std::size_t MaxGamepadCount { 8 };

        [[nodiscard]]
        Keyboard& get_keyboard() const noexcept;

        [[nodiscard]]
        Mouse& get_mouse() const noexcept;

        [[nodiscard]]
        Gamepad* get_gamepad_by_id(DeviceID id) const noexcept;

        [[nodiscard]]
        Gamepad* get_gamepad(std::size_t slot = 0) const noexcept;

        [[nodiscard]]
        std::array<Gamepad*, MaxGamepadCount> get_gamepads() const noexcept;

        [[nodiscard]]
        std::vector<Gamepad*> get_active_gamepads() const noexcept;

    private:
        void handle_events(const SDL_Event& event);
        void update();

        void handle_gamepad_added(DeviceID id);
        void handle_gamepad_removed(DeviceID id);

        std::unique_ptr<Keyboard> m_keyboard;
        std::unique_ptr<Mouse> m_mouse;
        std::array<std::optional<DeviceID>, MaxGamepadCount> m_gamepad_slots_to_ids;
        std::unordered_map<DeviceID, std::unique_ptr<Gamepad>> m_gamepads;
    };
} // namespace vn