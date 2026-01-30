#pragma once

#include <array>

namespace vn {
    template<std::size_t N>
    struct Buttons {
        std::array<bool, N> state_current {};
        std::array<bool, N> state_previous {};

        void update() {
            state_previous = state_current;
        }

        [[nodiscard]] bool is_pressed(std::size_t button_idx) const {
            return state_current[button_idx];
        }
        [[nodiscard]] bool is_just_pressed(std::size_t button_idx) const {
            return state_current[button_idx] && !state_previous[button_idx];
        }
        [[nodiscard]] bool is_just_released(std::size_t button_idx) const {
            return !state_current[button_idx] && state_previous[button_idx];
        }
    };
} // vn