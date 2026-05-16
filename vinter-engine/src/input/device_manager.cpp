#include "vinter/input/device_manager.hpp"

#include <ranges>

#include <SDL3/SDL.h>

#include "vinter/assert.hpp"
#include "vinter/input/gamepad.hpp"
#include "vinter/input/keyboard.hpp"
#include "vinter/input/mouse.hpp"

namespace vn {
    DeviceManager::DeviceManager() {
        m_keyboard = std::make_unique<Keyboard>();
        m_mouse = std::make_unique<Mouse>();

        // Scan existing gamepads on startup.
        int joystickCount = 0;
        SDL_JoystickID* joystickIds = SDL_GetJoysticks(&joystickCount);
        for (int i = 0; i < joystickCount; ++i) {
            HandleGamepadAdded(joystickIds[i]);
        }
        SDL_free(joystickIds);
    }

    auto DeviceManager::GetKeyboard() const noexcept -> Keyboard& {
        return *m_keyboard;
    }

    auto DeviceManager::GetMouse() const noexcept -> Mouse& {
        return *m_mouse;
    }

    auto DeviceManager::GetGamepads() const noexcept
        -> std::array<Gamepad*, DeviceManager::k_MaxGamepadCount> {
        // NOTE: We could cache this but the construction cost is minimal.
        std::array<Gamepad*, k_MaxGamepadCount> result {};

        for (std::size_t i = 0; i < k_MaxGamepadCount; i++)
            if (const auto& optionalId = m_gamepadSlots[i]; optionalId) {
                if (auto it = m_gamepads.find(*optionalId); it != m_gamepads.end()) {
                    result[i] = it->second.get();
                }
            }
        return result;
    }

    std::vector<Gamepad*> DeviceManager::GetActiveGamepads() const noexcept {
        std::vector<Gamepad*> result;
        result.reserve(k_MaxGamepadCount);

        for (const auto& gamepad : GetGamepads()) {
            if (gamepad != nullptr) {
                result.push_back(gamepad);
            }
        }
        return result;
    }

    auto DeviceManager::GetGamepadById(DeviceID id) const noexcept -> Gamepad* {
        if (const auto it = m_gamepads.find(id); it != m_gamepads.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    auto DeviceManager::GetGamepad(const std::size_t slot) const noexcept -> Gamepad* {
        VN_ASSERT(slot < k_MaxGamepadCount, "Gamepad slot out of range.");

        if (const auto& optionalId = m_gamepadSlots[slot]; optionalId) {
            return GetGamepadById(*optionalId);
        }
        return nullptr;
    }

    void DeviceManager::HandleEvents(const SDL_Event& event) {
        m_keyboard->HandleEvents(event);
        m_mouse->HandleEvents(event);

        if (event.type == SDL_EVENT_GAMEPAD_ADDED) {
            HandleGamepadAdded(event.gdevice.which);
        }
        if (event.type == SDL_EVENT_GAMEPAD_REMOVED) {
            HandleGamepadRemoved(event.gdevice.which);
        }

        for (const auto& gamepad : m_gamepads | std::views::values) {
            gamepad->HandleEvents(event);
        }
    }

    void DeviceManager::Update() {
        m_keyboard->Update();
        m_mouse->Update();
        for (const auto& gamepad : m_gamepads | std::views::values) {
            gamepad->Update();
        }
    }

    void DeviceManager::HandleGamepadAdded(const DeviceID id) {
        if (!SDL_IsGamepad(id)) {
            return;
        }

        const auto [it, inserted] = m_gamepads.try_emplace(id, std::make_unique<Gamepad>(id));

        if (!inserted) {
            return;
        }

        // Assign to first free slot.
        for (auto& slot : m_gamepadSlots) {
            if (!slot) {
                slot = id;
                break;
            }
        }
    }

    void DeviceManager::HandleGamepadRemoved(const DeviceID id) {
        // Erase from map, stop if not present.
        if (m_gamepads.erase(id) == 0) {
            return;
        }

        // Remove from slot assignment.
        for (auto& slot : m_gamepadSlots) {
            if (slot && *slot == id) {
                slot.reset();
                break;
            }
        }
    }
} // namespace vn