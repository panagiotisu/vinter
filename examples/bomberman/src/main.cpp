#include <exception>
#include <iostream>

#include "bomberman.hpp"

auto main() -> int {
    const vn::ProjectSettings project_settings {
        .window = {
            .flags = {
                .resizeable = true,
            }
        }
    };

    try {
        Bomberman bomberman(project_settings);
        bomberman.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Fatal error: unknown exception\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}