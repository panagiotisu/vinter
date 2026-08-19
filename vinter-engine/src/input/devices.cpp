#include <ranges>

#include <SDL3/SDL.h>

#include "vinter/input/devices.hpp"
#include "vinter/input/gamepad.hpp"
#include "vinter/input/keyboard.hpp"
#include "vinter/input/mouse.hpp"
#include "vinter/logger.hpp"

namespace vn {
    Devices::Devices()
        : m_keyboard(std::make_unique<Keyboard>())
        , m_mouse(std::make_unique<Mouse>()) {
        // Scan existing gamepads on startup.
        int joystick_count = 0;
        SDL_JoystickID* joystick_ids = SDL_GetJoysticks(&joystick_count);
        for (int i = 0; i < joystick_count; ++i) {
            handle_gamepad_added(joystick_ids[i]);
        }
        SDL_free(joystick_ids);
    }

    Keyboard& Devices::get_keyboard() const noexcept {
        return *m_keyboard;
    }

    Mouse& Devices::get_mouse() const noexcept {
        return *m_mouse;
    }

    std::array<Gamepad*, Devices::MaxGamepadCount> Devices::get_gamepads() const noexcept {
        // NOTE: We could cache this but the construction cost is minimal.
        std::array<Gamepad*, MaxGamepadCount> result {};

        for (std::size_t i = 0; i < MaxGamepadCount; i++) {
            if (const auto& optional_id = m_gamepad_slots_to_ids[i]; optional_id) {
                if (auto it = m_gamepads.find(*optional_id); it != m_gamepads.end()) {
                    result[i] = it->second.get();
                }
            }
        }
        return result;
    }

    std::vector<Gamepad*> Devices::get_active_gamepads() const noexcept {
        std::vector<Gamepad*> result;
        result.reserve(MaxGamepadCount);

        for (const auto& gamepad : get_gamepads()) {
            if (gamepad != nullptr) {
                result.push_back(gamepad);
            }
        }
        return result;
    }

    Gamepad* Devices::get_gamepad_by_id(DeviceID id) const noexcept {
        if (const auto it = m_gamepads.find(id); it != m_gamepads.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    Gamepad* Devices::get_gamepad(const std::size_t slot) const noexcept {
        VN_ASSERT(slot < MaxGamepadCount, "Gamepad slot out of range.");

        if (const auto& optional_id = m_gamepad_slots_to_ids[slot]; optional_id) {
            return get_gamepad_by_id(*optional_id);
        }
        return nullptr;
    }

    void Devices::handle_events(const SDL_Event& event) {
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

    void Devices::update() {
        m_keyboard->update();
        m_mouse->update();
        for (const auto& gamepad : m_gamepads | std::views::values) {
            gamepad->update();
        }
    }

    void Devices::handle_gamepad_added(const DeviceID id) {
        if (!SDL_IsGamepad(id)) {
            return;
        }

        const auto [it, inserted] = m_gamepads.try_emplace(id, std::make_unique<Gamepad>(id));

        if (!inserted) {
            return;
        }

        // Assign to first free slot.
        for (std::size_t slot = 0; slot < m_gamepad_slots_to_ids.size(); slot++) {
            if (!m_gamepad_slots_to_ids[slot].has_value()) {
                m_gamepad_slots_to_ids[slot] = id;
                VN_INFO("Gamepad with ID {} connected to slot {}.", id, slot);
                break;
            }
        }
    }

    void Devices::handle_gamepad_removed(const DeviceID id) {
        // Erase from map, stop if not present.
        if (m_gamepads.erase(id) == 0) {
            return;
        }

        // Remove from slot assignment.
        for (std::size_t slot = 0; slot < m_gamepad_slots_to_ids.size(); slot++) {
            if (m_gamepad_slots_to_ids[slot].has_value()
                && m_gamepad_slots_to_ids[slot].value() == id) {
                m_gamepad_slots_to_ids[slot].reset();
                VN_INFO("Gamepad with ID {} disconnected from slot {}.", id, slot);
                break;
            }
        }
    }
} // namespace vn