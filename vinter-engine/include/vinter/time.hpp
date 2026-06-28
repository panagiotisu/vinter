#pragma once

#include <chrono>
#include <cstdint>

namespace vn {
    class Time {
        friend class Engine;

        using Clock = std::chrono::steady_clock;
        using TimePoint = Clock::time_point;
        using Duration = std::chrono::duration<double>;

    public:
        Time();

        [[nodiscard]]
        auto get_delta_time() const noexcept -> float;

        [[nodiscard]]
        auto get_unscaled_delta_time() const noexcept -> float;

        [[nodiscard]]
        auto get_elapsed_time() const noexcept -> float;

        [[nodiscard]]
        auto get_unscaled_elapsed_time() const noexcept -> float;

        [[nodiscard]]
        auto get_fps() const -> float;

        [[nodiscard]]
        auto get_time_scale() const noexcept -> float;

        void set_time_scale(float scale);

        void pause();

        void unpause();

        void toggle_pause();

        [[nodiscard]]
        auto is_paused() const noexcept -> bool;

        void reset();

    private:
        void update();
        void update_delta();
        void update_fps();

        TimePoint m_start_time;
        TimePoint m_last_frame_time;

        float m_time_scale {1.f};
        float m_delta_time {};
        float m_unscaled_delta_time {};
        float m_max_delta_time {0.1f};
        float m_elapsed_time {};
        float m_unscaled_elapsed_time {};
        float m_fps {};
        bool m_paused {false};
    };
} // namespace vn