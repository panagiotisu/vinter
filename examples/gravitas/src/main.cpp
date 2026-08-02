#include "gravitas.hpp"

auto main() -> int {
    const vn::ProjectSettings project_settings {
        .window = {
            .title = "Gravitas",
            .flags = {
                .resizeable = true,
            },
        },
        .renderer = {
            .backend = vn::RendererSettings::Backend::Automatic,
            .vsync = true,
        },
    };

    Gravitas gravitas(project_settings);
    gravitas.run();
}