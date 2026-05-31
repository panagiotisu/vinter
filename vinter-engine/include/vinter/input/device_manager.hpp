#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

union SDL_Event;

namespace vn
{
    class Keyboard;
    class Mouse;
    class Gamepad;

    using DeviceID = std::uint32_t;

    class DeviceManager
    {
        friend class Engine;

    public:
        DeviceManager();

        static constexpr std::size_t k_max_gamepad_count {8};

        [[nodiscard]]
        auto get_keyboard() const noexcept -> Keyboard&;

        [[nodiscard]]
        auto get_mouse() const noexcept -> Mouse&;

        [[nodiscard]]
        auto get_gamepad_by_id(DeviceID id) const noexcept -> Gamepad*;

        [[nodiscard]]
        auto get_gamepad(std::size_t slot = 0) const noexcept -> Gamepad*;

        [[nodiscard]]
        auto get_gamepads() const noexcept -> std::array<Gamepad*, k_max_gamepad_count>;

        [[nodiscard]]
        auto get_active_gamepads() const noexcept -> std::vector<Gamepad*>;

    private:
        void handle_events(const SDL_Event& event);
        void update();

        void handle_gamepad_added(DeviceID id);
        void handle_gamepad_removed(DeviceID id);

        std::unique_ptr<Keyboard> m_keyboard;
        std::unique_ptr<Mouse> m_mouse;
        std::array<std::optional<DeviceID>, k_max_gamepad_count> m_gamepad_slots;
        std::unordered_map<DeviceID, std::unique_ptr<Gamepad>> m_gamepads;
    };
} // namespace vn