#include "bomberman.hpp"

int main() {
    const vn::ProjectSettings project_settings {
        .window = {
            .flags = {
                .resizeable = true,
            }
        }
    };
    Bomberman bomberman(project_settings);
    bomberman.run();

    return 0;
}