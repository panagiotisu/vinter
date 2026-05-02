#pragma once

#include <cstdint>

namespace vn {
    class Time {
        friend class Engine;

    public:
        Time();

        [[nodiscard]]
        auto GetDelta() const -> float;

        [[nodiscard]]
        auto GetFps() const -> float;

    private:
        void Update();

        std::uint64_t m_tickPrevious {0};
        std::uint64_t m_tickCurrent {0};
        std::uint64_t m_frequency {0};
        float m_delta {0.f};
    };
} // namespace vn