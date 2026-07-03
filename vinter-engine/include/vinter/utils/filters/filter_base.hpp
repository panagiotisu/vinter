#pragma once

#include <cstddef>
#include <vector>

namespace vn {
    /**
     * @brief Base class for all windowed time series filters.
     *
     * @tparam T Strictly arithmetic type.
     */
    template <typename T>
    requires std::is_arithmetic_v<T>
    class FilterBase {
    public:
        explicit FilterBase(std::size_t window_size = 60) : m_window_size(window_size) {
            m_samples.reserve(window_size);
        }

        virtual ~FilterBase() = default;

        virtual void add_sample(T sample) = 0;

        [[nodiscard]]
        virtual auto get_value() const -> T = 0;

        virtual void reset() = 0;

    protected:
        std::size_t m_window_size {};
        std::vector<T> m_samples;
    };
} // namespace vn