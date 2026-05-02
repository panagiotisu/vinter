#pragma once

#include <array>

namespace vn {
    template <std::size_t N>
    struct ButtonStates {
        std::array<bool, N> current {};
        std::array<bool, N> previous {};

        void Refresh() {
            previous = current;
        }

        [[nodiscard]]
        auto IsPressed(std::size_t buttonIdx) const -> bool {
            return current[buttonIdx];
        }

        [[nodiscard]]
        auto IsJustPressed(std::size_t buttonIdx) const -> bool {
            return current[buttonIdx] && !previous[buttonIdx];
        }

        [[nodiscard]]
        auto IsJustReleased(std::size_t buttonIdx) const -> bool {
            return !current[buttonIdx] && previous[buttonIdx];
        }
    };
} // namespace vn