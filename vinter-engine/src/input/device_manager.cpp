#include "vinter/input/device_manager.hpp"

#include <ranges>
#include <SDL3/SDL.h>

#include <vinter/input/keyboard.hpp>
#include <vinter/input/mouse.hpp>
#include <vinter/input/gamepad.hpp>

namespace vn {
    DeviceManager::DeviceManager() {
        m_keyboard = std::make_unique<Keyboard>();
        m_mouse = std::make_unique<Mouse>();

        // Scan existing gamepads on startup.
        int num_gamepads = 0;
        SDL_GetJoysticks(&num_gamepads);
        for (int i = 0; i < num_gamepads; i++) {
            handle_gamepad_added(i);
        }
    }

    Keyboard& DeviceManager::get_keyboard() const noexcept {
        return *m_keyboard;
    }

    Mouse& DeviceManager::get_mouse() const noexcept {
        return *m_mouse;
    }

    std::vector<Gamepad*> DeviceManager::get_gamepads() const noexcept {
        std::vector<Gamepad*> result;
        result.reserve(m_gamepads.size());
        for (const auto& gamepad : m_gamepads | std::views::values)
            result.push_back(gamepad.get());
        return result;
    }

    Gamepad* DeviceManager::get_gamepad(const unsigned int gamepad_id) const noexcept {
        const auto it = m_gamepads.find(gamepad_id);
        return it != m_gamepads.end() ? it->second.get() : nullptr;
    }

    void DeviceManager::handle_events(const SDL_Event& event) {
        m_keyboard->handle_events(event);
        m_mouse->handle_events(event);

        if (event.type == SDL_EVENT_GAMEPAD_ADDED) {
            handle_gamepad_added(event.gdevice.which);
        }
        if (event.type == SDL_EVENT_GAMEPAD_REMOVED) {
            m_gamepads.erase(event.gdevice.which);
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

    void DeviceManager::handle_gamepad_added(unsigned int gamepad_id) {
        if (SDL_IsGamepad(gamepad_id)) {
            m_gamepads.try_emplace(gamepad_id, std::make_unique<Gamepad>(gamepad_id));
        }
    }
} // vn