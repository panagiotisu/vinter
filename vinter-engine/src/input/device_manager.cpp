#include "vinter/input/device_manager.hpp"

#include <ranges>
#include <cassert>

#include <SDL3/SDL.h>

#include "vinter/input/keyboard.hpp"
#include "vinter/input/mouse.hpp"
#include "vinter/input/gamepad.hpp"

namespace vn {
    DeviceManager::DeviceManager() {
        m_keyboard = std::make_unique<Keyboard>();
        m_mouse = std::make_unique<Mouse>();

        // Scan existing gamepads on startup.
        int joystick_count = 0;
        SDL_JoystickID* joystick_ids = SDL_GetJoysticks(&joystick_count);
        for (int i = 0; i < joystick_count; ++i) {
            handle_gamepad_added(joystick_ids[i]);
        }
        SDL_free(joystick_ids);
    }

    Keyboard& DeviceManager::get_keyboard() const noexcept {
        return *m_keyboard;
    }

    Mouse& DeviceManager::get_mouse() const noexcept {
        return *m_mouse;
    }

    std::array<Gamepad*, DeviceManager::MaxGamepadCount> DeviceManager::get_gamepads() const noexcept {
        // NOTE: We could cache this but the construction cost is minimal.
        std::array<Gamepad*, MaxGamepadCount> result {};

        for (std::size_t i = 0; i < MaxGamepadCount; i++)
            if (const auto& optional_id = m_gamepad_slots[i]; optional_id) {
                if (auto it = m_gamepads.find(*optional_id); it != m_gamepads.end()) {
                    result[i] = it->second.get();
                }
            }
        return result;
    }

    std::vector<Gamepad*> DeviceManager::get_active_gamepads() const noexcept {
        std::vector<Gamepad*> result;
        result.reserve(MaxGamepadCount);

        for (const auto& gamepad : get_gamepads()) {
            if (gamepad) result.push_back(gamepad);
        }
        return result;
    }

    Gamepad* DeviceManager::get_gamepad_by_id(DeviceID id) const noexcept {
        if (const auto it = m_gamepads.find(id); it != m_gamepads.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    Gamepad* DeviceManager::get_gamepad(const std::size_t slot) const noexcept {
        assert(slot < MaxGamepadCount && "Gamepad slot out of range.");

        if (const auto& optional_id = m_gamepad_slots[slot]; optional_id) {
            return get_gamepad_by_id(*optional_id);
        }
        return nullptr;
    }

    void DeviceManager::handle_events(const SDL_Event& event) {
        m_keyboard->handle_events(event);
        m_mouse->handle_events(event);

        if (event.type == SDL_EVENT_GAMEPAD_ADDED) {
            handle_gamepad_added(event.gdevice.which);
        }
        if (event.type == SDL_EVENT_GAMEPAD_REMOVED) {
            handle_gamepad_removed(event.gdevice.which);
        }

        for (const auto& gamepad : m_gamepads | std::views::values) {
            gamepad->handle_events(event);
        }
    }

    void DeviceManager::update() {
        m_keyboard->update();
        m_mouse->update();
        for (const auto& gamepad : m_gamepads | std::views::values) {
            gamepad->update();
        }
    }

    void DeviceManager::handle_gamepad_added(const DeviceID id) {
        if (!SDL_IsGamepad(id)) return;

        const auto [it, inserted] = m_gamepads.try_emplace(id, std::make_unique<Gamepad>(id));

        if (!inserted) return;

        // Assign to first free slot.
        for (auto& slot : m_gamepad_slots) {
            if (!slot) {
                slot = id;
                break;
            }
        }
    }

    void DeviceManager::handle_gamepad_removed(const DeviceID id) {
        // Erase from map, stop if not present.
        if (m_gamepads.erase(id) == 0) return;

        // Remove from slot assignment.
        for (auto& slot : m_gamepad_slots) {
            if (slot && *slot == id) {
                slot.reset();
                break;
            }
        }
    }
} // vn