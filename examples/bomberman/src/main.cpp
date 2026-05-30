#include "bomberman.hpp"

auto main() -> int {
    const vn::ProjectSettings projectSettings {
        .window = {
            .flags = {
                .resizeable = true,
            }
        }
    };
    Bomberman bomberman(projectSettings);
    bomberman.Run();

    return 0;
}