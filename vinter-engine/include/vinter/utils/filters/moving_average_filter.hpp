#pragma once

#include "vinter/utils/filters/filter_base.hpp"

namespace vn {
    template <typename T>
    class MovingAverageFilter : public FilterBase<T> {
    public:
        explicit MovingAverageFilter(std::size_t window_size = 60) : FilterBase<T>(window_size) {
        }

        void add_sample(T sample) override {
            this->m_samples.push_back(sample);
            m_running_sum += sample;

            if (this->m_samples.size() > this->m_window_size) {
                m_running_sum -= this->m_samples.front();
                this->m_samples.erase(this->m_samples.begin());
            }
        }

        [[nodiscard]]
        auto get_value() const -> T override {
            if (this->m_samples.empty()) {
                return T {};
            }
            return m_running_sum / static_cast<T>(this->m_samples.size());
        }

        void reset() override {
            this->m_samples.clear();
            m_running_sum = T {};
        }

    private:
        T m_running_sum {T {}};
    };
} // namespace vn