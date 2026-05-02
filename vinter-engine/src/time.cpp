#include "vinter/time.hpp"

#include <SDL3/SDL.h>

namespace vn {
    Time::Time()
        : m_tickCurrent(SDL_GetPerformanceCounter())
        , m_frequency(SDL_GetPerformanceFrequency()) {
    }

    void Time::Update() {
        m_tickPrevious = m_tickCurrent;
        m_tickCurrent = SDL_GetPerformanceCounter();

        m_delta =
            static_cast<float>(m_tickCurrent - m_tickPrevious) / static_cast<float>(m_frequency);
    }

    auto Time::GetDelta() const -> float {
        return m_delta;
    }

    auto Time::GetFps() const -> float {
        return m_delta > 0.0f ? 1.0f / m_delta : 0.0f;
    }
} // namespace vn