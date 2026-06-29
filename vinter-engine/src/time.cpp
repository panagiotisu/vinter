#include "vinter/time.hpp"

namespace vn {
    Time::Time() : m_start_time(Clock::now()), m_last_frame_time(m_start_time) {
    }

    auto Time::get_delta_time() const noexcept -> float {
        return m_delta_time;
    }

    auto Time::get_unscaled_delta_time() const noexcept -> float {
        return m_unscaled_delta_time;
    }

    auto Time::get_elapsed_time() const noexcept -> float {
        return m_elapsed_time;
    }

    auto Time::get_unscaled_elapsed_time() const noexcept -> float {
        return m_unscaled_elapsed_time;
    }

    auto Time::get_wall_clock_time() const noexcept -> float {
        return m_wall_clock_time;
    }

    auto Time::get_fps() const -> float {
        if (m_unscaled_delta_time <= 0.f) {
            return 0.f;
        }
        return 1.f / m_unscaled_delta_time;
    }

    auto Time::get_time_scale() const noexcept -> float {
        return m_time_scale;
    }

    void Time::set_time_scale(float scale) {
        m_time_scale = std::max(0.f, scale);
    }

    void Time::pause() {
        m_paused = true;
    }

    void Time::unpause() {
        if (m_paused) {
            m_paused = false;
            m_last_frame_time = Clock::now();
        }
    }

    void Time::toggle_pause() {
        if (m_paused) {
            unpause();
        } else {
            pause();
        }
    }

    auto Time::is_paused() const noexcept -> bool {
        return m_paused;
    }

    void Time::reset() {
        const TimePoint now = Clock::now();
        m_start_time = now;
        m_last_frame_time = now;

        m_delta_time = 0.f;
        m_unscaled_delta_time = 0.f;
        m_elapsed_time = 0.f;
        m_unscaled_elapsed_time = 0.f;
        m_fps = 0.f;
    }

    void Time::update() {
        const TimePoint current_time {Clock::now()};

        double raw_delta_time {Duration(current_time - m_last_frame_time).count()};
        m_last_frame_time = current_time;

        m_unscaled_delta_time = static_cast<float>(raw_delta_time);
        m_unscaled_delta_time = std::min(m_unscaled_delta_time, m_max_delta_time);

        if (m_paused) {
            m_delta_time = 0.f;
        } else {
            m_delta_time = m_unscaled_delta_time * m_time_scale;

            // Unscaled elapsed time is affected by pausing but is not scaled.
            m_unscaled_elapsed_time += m_unscaled_delta_time;

            // Elapsed time is affected by pausing and is scaled.
            m_elapsed_time += m_delta_time;
        }

        // Wall clock remains unaffected by both scaling and pausing.
        m_wall_clock_time += m_unscaled_delta_time;
    }
} // namespace vn