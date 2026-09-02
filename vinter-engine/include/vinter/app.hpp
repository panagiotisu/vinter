#pragma once

#include <memory>

// TODO: Place these in a fwd.hpp.
#include "vinter/assets/font_manager.hpp"
#include "vinter/assets/texture_manager.hpp"
#include "vinter/color.hpp"
#include "vinter/ecs/database.hpp"
#include "vinter/ecs/entity.hpp"
#include "vinter/ecs/system.hpp"
#include "vinter/graphics/font.hpp"
#include "vinter/graphics/renderer.hpp"
#include "vinter/graphics/sprite.hpp"
#include "vinter/graphics/texture.hpp"
#include "vinter/input/devices.hpp"
#include "vinter/input/gamepad.hpp"
#include "vinter/input/input_map.hpp"
#include "vinter/input/keyboard.hpp"
#include "vinter/input/mouse.hpp"
#include "vinter/logger.hpp"
#include "vinter/settings/project_settings.hpp"
#include "vinter/spatial/angle.hpp"
#include "vinter/spatial/geometry.hpp"
#include "vinter/spatial/transform.hpp"
#include "vinter/time.hpp"
#include "vinter/window.hpp"

namespace vn {
    class App {
    public:
        explicit App(const ProjectSettings& project_settings);
        virtual ~App();

        void run();

    protected:
        virtual void load() {
        }

        virtual void update(float delta) {
        }

        virtual void render() {
        }

        virtual void unload() {
        }

        void quit();

        [[nodiscard]]
        Window& get_window() noexcept;

        [[nodiscard]]
        Renderer& get_renderer() noexcept;

        [[nodiscard]]
        Time& get_time() noexcept;

        [[nodiscard]]
        Devices& get_devices() noexcept;

        [[nodiscard]]
        InputMap& get_input() noexcept;

        [[nodiscard]]
        ecs::Database& get_ecs() noexcept;

        [[nodiscard]]
        ecs::SystemQueue& get_systems() noexcept;

        [[nodiscard]]
        TextureManager& get_textures() noexcept;

        [[nodiscard]]
        FontManager& get_fonts() noexcept;

    protected:
        virtual void handle_debug_gui_events(const SDL_Event& event) {
        }

    private:
        class SDLContext {
        public:
            explicit SDLContext(int init_flags);
            ~SDLContext();
        };

    private:
        std::unique_ptr<SDLContext> m_ctx {};
        std::unique_ptr<TextureManager> m_textures {};
        std::unique_ptr<FontManager> m_fonts {};
        std::unique_ptr<Window> m_window {};
        std::unique_ptr<Renderer> m_renderer {};
        std::unique_ptr<Time> m_time {};
        std::unique_ptr<Devices> m_devices {};
        std::unique_ptr<InputMap> m_input {};
        std::unique_ptr<ecs::Database> m_database {};
        std::unique_ptr<ecs::SystemQueue> m_systems {};

        bool m_running { false };
    };
} // namespace vn