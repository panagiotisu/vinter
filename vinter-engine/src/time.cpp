#include "vinter/time.hpp"

#include <SDL3/SDL.h>

namespace vn
{
    Time::Time()
        : m_tick_current(SDL_GetPerformanceCounter())
        , m_frequency(SDL_GetPerformanceFrequency())
    {
    }

    void Time::update()
    {
        m_tick_previous = m_tick_current;
        m_tick_current = SDL_GetPerformanceCounter();

        m_delta = static_cast<float>(m_tick_current - m_tick_previous)
                  / static_cast<float>(m_frequency);
    }

    auto Time::get_delta() const -> float
    {
        return m_delta;
    }

    auto Time::get_fps() const -> float
    {
        return m_delta > 0.0f ? 1.0f / m_delta : 0.0f;
    }
} // namespace vn