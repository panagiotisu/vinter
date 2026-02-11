#pragma once

#include <array>

namespace vn {
    template<std::size_t N>
    struct ButtonStates {
        std::array<bool, N> current {};
        std::array<bool, N> previous {};

        void refresh() {
            previous = current;
        }

        [[nodiscard]] bool is_pressed(std::size_t button_idx) const {
            return current[button_idx];
        }
        [[nodiscard]] bool is_just_pressed(std::size_t button_idx) const {
            return current[button_idx] && !previous[button_idx];
        }
        [[nodiscard]] bool is_just_released(std::size_t button_idx) const {
            return !current[button_idx] && previous[button_idx];
        }
    };
} // vn