#pragma once

#include <chrono>

#include "vinter/utils/filters/moving_average_filter.hpp"

namespace vn {
    class Time {
        friend class Engine;

        using Clock = std::chrono::steady_clock;
        using TimePoint = Clock::time_point;
        using Duration = std::chrono::duration<double>;

    public:
        Time();

        /**
         * @return The time passed between two successive frames.
         *
         * @note Is affected by scaling.
         */
        [[nodiscard]]
        auto get_delta_time() const noexcept -> float;

        /**
         * @return The time passed between two successive frames.

         * @note Is NOT affected by scaling.
         */
        [[nodiscard]]
        auto get_unscaled_delta_time() const noexcept -> float;

        /**
         * @return The time passed between two successive frames.
         *
         * @note Is affected by scaling.
         * @note Stops accumulating when the game is paused.
         */
        [[nodiscard]]
        auto get_elapsed_time() const noexcept -> float;

        /**
         * @return The time passed since the game launch, or the last reset.
         *
         * @note Is NOT affected by scaling.
         * @note Stops accumulating when the game is paused
         */
        [[nodiscard]]
        auto get_unscaled_elapsed_time() const noexcept -> float;

        /**
         * @return The time passed since the game launch, or the last reset.
         *
         * @note Is NOT affected by scaling.
         * @note Continues to accumulate when the game is paused.
         */
        [[nodiscard]]
        auto get_wall_clock_time() const noexcept -> float;

        /**
         * @return The current FPS calculated between current and last frame.
         *
         * @note Useful for time-sensitive and accurate statistics.
         */
        [[nodiscard]]
        auto get_instant_fps() const -> float;

        /**
         * @return The averaged FPS filtered by m_fps_filter.
         *
         * @note Useful fur user-facing FPS infos.
         */
        [[nodiscard]]
        auto get_filtered_fps() const -> std::uint32_t;

        /**
         * @return The scale at which time passes, defaults to 1.0.
         */
        [[nodiscard]]
        auto get_time_scale() const noexcept -> float;

        /**
         * @brief Sets the desired scale at which time passes.
         *
         * @param scale The desired time scale.
         */
        void set_time_scale(float scale);

        /**
         * @brief Pauses the game unconditionally.
         */
        void pause();

        /**
         * @brief Unpauses the game if not paused.
         */
        void unpause();

        /**
         * @brief Toggles between paused and unpaused game states.
         */
        void toggle_pause();

        /**
         * @return 'true' if the game is paused, and 'false' otherwise.
         */
        [[nodiscard]]
        auto is_paused() const noexcept -> bool;

        /**
         * @brief Resets all accumulated times.
         */
        void reset();

    private:
        void update();
        void update_timers();
        void update_fps();

    private:
        TimePoint m_start_time {};
        TimePoint m_last_frame_time {};

        float m_time_scale { 1.f };

        float m_delta_time {};
        float m_unscaled_delta_time {};
        float m_max_delta_time { 0.1f };

        float m_elapsed_time {};
        float m_unscaled_elapsed_time {};
        float m_wall_clock_time {};

        float m_fps {};
        std::unique_ptr<MovingAverageFilter<float>> m_frame_time_filter {};

        bool m_paused { false };
    };
} // namespace vn