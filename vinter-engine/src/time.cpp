#include "vinter/time.hpp"

#include <SDL3/SDL.h>

namespace vn {
    Time::Time()
        : m_tick_current(SDL_GetPerformanceCounter())
        , m_frequency(SDL_GetPerformanceFrequency()) {
    }

    void Time::update() {
        m_tick_previous = m_tick_current;
        m_tick_current = SDL_GetPerformanceCounter();

        m_delta = static_cast<float>(m_tick_current - m_tick_previous) /
                  static_cast<float>(m_frequency);
    }

    float Time::get_delta() const { return m_delta; }

    float Time::get_fps() const {
        return m_delta > 0.0f ? 1.0f / m_delta : 0.0f;
    }
} // vn