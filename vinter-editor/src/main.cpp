#include "editor.hpp"

int main() {
    Editor editor({
        .window = {
            .title = "Vinter Engine",
            .flags = {
                .resizeable = true,
            },
        },
        .renderer = {
            .backend = vn::RendererSettings::Backend::Automatic,
            .vsync = vn::RendererSettings::VSyncMode::Adaptive,
        },
    });
    editor.run();
}
