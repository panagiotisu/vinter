#include "bomberman.hpp"

auto main() -> int {
    const vn::ProjectSettings project_settings {
        .window = {
            .flags = {
                .resizeable = true,
            }
        }
    };

    Bomberman bomberman(project_settings);
    bomberman.run();
}