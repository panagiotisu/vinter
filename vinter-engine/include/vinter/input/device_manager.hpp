#pragma once

#include <array>
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

    class DeviceManager {
        friend class Engine;

    public:
        DeviceManager();

        static constexpr std::size_t k_MaxGamepadCount {8};

        [[nodiscard]]
        auto GetKeyboard() const noexcept -> Keyboard&;

        [[nodiscard]]
        auto GetMouse() const noexcept -> Mouse&;

        [[nodiscard]]
        auto GetGamepadById(DeviceID id) const noexcept -> Gamepad*;

        [[nodiscard]]
        auto GetGamepad(std::size_t slot = 0) const noexcept -> Gamepad*;

        [[nodiscard]]
        auto GetGamepads() const noexcept -> std::array<Gamepad*, k_MaxGamepadCount>;

        [[nodiscard]]
        auto GetActiveGamepads() const noexcept -> std::vector<Gamepad*>;

    private:
        void HandleEvents(const SDL_Event& event);
        void Update();

        void HandleGamepadAdded(DeviceID id);
        void HandleGamepadRemoved(DeviceID id);

        std::unique_ptr<Keyboard> m_keyboard;
        std::unique_ptr<Mouse> m_mouse;
        std::array<std::optional<DeviceID>, k_MaxGamepadCount> m_gamepadSlots;
        std::unordered_map<DeviceID, std::unique_ptr<Gamepad>> m_gamepads;
    };
} // namespace vn