#pragma once

#include <cstdint>

namespace vn {
    class Time {
        friend class Engine;

    public:
        Time();

        [[nodiscard]] float get_delta() const;
        [[nodiscard]] float get_fps() const;

    private:
        void update();

        std::uint64_t m_tick_previous { 0 };
        std::uint64_t m_tick_current { 0 };
        std::uint64_t m_frequency { 0 };
        float m_delta { 0.f };
    };
} // vn