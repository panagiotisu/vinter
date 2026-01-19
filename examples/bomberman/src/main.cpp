#include "bomberman.hpp"

int main() {
    const vn::ProjectSettings project_settings {};
    Bomberman bomberman(project_settings);
    bomberman.run();

    return 0;
}