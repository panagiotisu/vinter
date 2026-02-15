#include "vinter/input/deadzones.hpp"

#include <cmath>

namespace vn::deadzone {
    void axial(float& axis, const float deadzone) {
        if (axis < deadzone) axis = 0.f;
    }

    void axial(float& axis_x, float& axis_y, const float deadzone) {
        axial(axis_x, deadzone);
        axial(axis_y, deadzone);
    }

    void radial(float& axis_x, float& axis_y, const float deadzone) {
        const float magnitude_squared = axis_x * axis_x + axis_y * axis_y;
        if (magnitude_squared < deadzone * deadzone) {
            axis_x = 0.f;
            axis_y = 0.f;
        }
    }

    static float map_range(
        const float value,
        const float old_min, const float old_max,
        const float new_min, const float new_max
    ) {
        return (new_min + (new_max - new_min) * (value - old_min) / (old_max - old_min));
    }

    void smooth(float& axis, const float deadzone) {
        axial(axis, deadzone);
        axis = map_range(axis, deadzone, 1.f, 0.f, 1.f);
    }

    void smooth(float& axis_x, float& axis_y, const float deadzone) {
        const float magnitude_squared = axis_x * axis_x + axis_y * axis_y;
        if (magnitude_squared < deadzone * deadzone) {
            axis_x = 0.f;
            axis_y = 0.f;
        }

        const float magnitude = std::sqrt(magnitude_squared);
        const float remapped_magnitude = map_range(magnitude, deadzone, 1.f, 0.f, 1.f);

        axis_x *= remapped_magnitude / magnitude;
        axis_y *= remapped_magnitude / magnitude;
    }
} //vn::deadzone