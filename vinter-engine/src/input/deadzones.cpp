#include "vinter/input/deadzones.hpp"

namespace vn::deadzone {
    float axial(const float axis_1d, const float deadzone) {
        if (axis_1d < deadzone) return 0.f;
        return axis_1d;
    }

    glm::vec2 axial(const glm::vec2 axis_2d, const float deadzone) {
        return glm::vec2 {
            axial(axis_2d.x, deadzone),
            axial(axis_2d.y, deadzone)
        };
    }

    glm::vec2 radial(const glm::vec2 axis_2d, const float deadzone) {
        const float magnitude_squared = glm::dot(axis_2d, axis_2d);
        if (magnitude_squared < deadzone * deadzone) return glm::vec2 { 0.f, 0.f };
        return axis_2d;
    }

    static float map_range(
        const float value,
        const float old_min, const float old_max,
        const float new_min, const float new_max
    ) {
        return (new_min + (new_max - new_min) * (value - old_min) / (old_max - old_min));
    }

    float smooth(const float axis_1d, const float deadzone) {
        if (axis_1d < deadzone) return 0.f;
        return map_range(axis_1d, deadzone, 1.f, 0.f, 1.f);
    }

    glm::vec2 smooth(const glm::vec2 axis_2d, const float deadzone) {
        const float magnitude_squared = glm::dot(axis_2d, axis_2d);
        if (magnitude_squared < deadzone * deadzone) return glm::vec2 { 0.f, 0.f };

        const float magnitude = std::sqrt(magnitude_squared);
        const glm::vec2 axis_2d_normalized = axis_2d / magnitude;
        return axis_2d_normalized * map_range(magnitude, deadzone, 1.f, 0.f, 1.f);
    }
} //vn::deadzone