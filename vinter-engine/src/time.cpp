#include "vinter/time.hpp"

namespace vn {
    Time::Time()
        : m_start_time(Clock::now())
        , m_last_frame_time(m_start_time)
        , m_frame_time_filter(std::make_unique<MovingAverageFilter<float>>(120)) {
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

    auto Time::get_instant_fps() const -> float {
        if (m_unscaled_delta_time <= 0.f) {
            return 0.f;
        }
        return 1.f / m_unscaled_delta_time;
    }

    auto Time::get_filtered_fps() const -> std::uint32_t {
        const auto filtered_fps {m_frame_time_filter->get_value()};
        if (filtered_fps <= 0.f) {
            return 0;
        }
        return static_cast<std::uint32_t>(1.f / filtered_fps);
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
        m_wall_clock_time = 0.f;

        m_fps = 0.f;
        m_frame_time_filter->reset();
    }

    void Time::update() {
        update_timers();
        update_fps();
    }

    void Time::update_timers() {
        const TimePoint current_time {Clock::now()};

        const double raw_delta_time {Duration(current_time - m_last_frame_time).count()};
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

    void Time::update_fps() {
        m_frame_time_filter->add_sample(get_unscaled_delta_time());
    }
} // namespace vn