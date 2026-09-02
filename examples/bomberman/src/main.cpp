#include "bomberman.hpp"

auto main() -> int {
    const vn::ProjectSettings project_settings {
        .window = {
            .title = "Atomic Bomberman",
            .flags = {
                .resizeable = true,
            },
        },
        .renderer = {
            .backend = vn::RendererSettings::Backend::Automatic,
            .vsync = vn::RendererSettings::VSyncMode::Disabled,
        },
        .textures = {
            .global_scale_mode = vn::TextureSettings::ScaleMode::PixelArt,
        }
    };

    Bomberman bomberman(project_settings);
    bomberman.run();
}