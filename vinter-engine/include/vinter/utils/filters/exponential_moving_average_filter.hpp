#pragma once

#include "vinter/utils/filters/filter_base.hpp"

namespace vn {
    template <typename T>
    class ExponentialMovingAverageFilter : public FilterBase<T> {
    public:
        explicit ExponentialMovingAverageFilter(std::size_t window_size = 60)
            : FilterBase<T>(window_size) {
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
        T get_value() const override {
            if (this->m_samples.empty()) {
                return T {};
            }
            return m_running_sum / static_cast<T>(this->m_samples.size());
        }

        void reset() {
        }

    private:
        T m_running_sum {};
        T m_weighted_sum {};
        T m_total_weight {};
    };
} // namespace vn