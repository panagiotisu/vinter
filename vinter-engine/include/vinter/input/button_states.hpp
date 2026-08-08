#pragma once

#include <vector>

namespace vn {
    struct ButtonStates {
        std::vector<bool> current {};
        std::vector<bool> previous {};

        explicit ButtonStates(std::size_t button_count)
            : current(button_count, false)
            , previous(button_count, false) {
        }

        void refresh() {
            previous = current;
        }

        [[nodiscard]]
        bool is_pressed(std::size_t button_idx) const noexcept {
            return current[button_idx];
        }

        [[nodiscard]]
        bool is_just_pressed(std::size_t button_idx) const noexcept {
            return current[button_idx] && !previous[button_idx];
        }

        [[nodiscard]]
        bool is_just_released(std::size_t button_idx) const noexcept {
            return !current[button_idx] && previous[button_idx];
        }
    };
} // namespace vn