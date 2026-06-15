#include "bomberman.hpp"

auto main() -> int {
    const vn::ProjectSettings project_settings {
        .window = {
            .title = "Atomic Bomberman",
            .flags = {
                .resizeable = true,
            }
        }
    };

    Bomberman bomberman(project_settings);
    bomberman.run();
}