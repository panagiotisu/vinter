#pragma once

#include <vector>

namespace vn {
    template <typename T>
    requires std::is_arithmetic_v<T>
    struct InputStates {
        std::vector<T> current {};
        std::vector<T> previous {};

        explicit InputStates(std::size_t button_count)
            : current(button_count)
            , previous(button_count) {
        }

        void refresh() {
            previous = current;
        }

        [[nodiscard]]
        bool is_pressed(std::size_t input_index) const noexcept {
            return is_active(current[input_index]);
        }

        [[nodiscard]]
        bool is_just_pressed(std::size_t input_index) const noexcept {
            return is_active(current[input_index]) && !is_active(previous[input_index]);
        }

        [[nodiscard]]
        bool is_just_released(std::size_t input_index) const noexcept {
            return !is_active(current[input_index]) && is_active(previous[input_index]);
        }

    private:
        [[nodiscard]]
        bool is_active(T input_state) const noexcept {
            return input_state > 0;
        }
    };
} // namespace vn