#pragma once

#include <glm/glm.hpp>

// Various deadzone-applying algorithms based on the following article:
// https://github.com/Minimuino/thumbstick-deadzones/blob/master/README.md

namespace vn::deadzone {
    float axial(float axis_1d, float deadzone);

    glm::vec2 axial(glm::vec2 axis_2d, float deadzone);

    glm::vec2 radial(glm::vec2 axis_2d, float deadzone);

    float smooth(float axis_1d, float deadzone);

    glm::vec2 smooth(glm::vec2 axis_2d, float deadzone);
} // vn::deadzone
