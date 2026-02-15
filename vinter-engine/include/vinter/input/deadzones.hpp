#pragma once

// Various deadzone-applying algorithms based on the following article:
// https://github.com/Minimuino/thumbstick-deadzones/blob/master/README.md

namespace vn::deadzone {
    void axial(float& axis, float deadzone);

    void axial(float& axis_x, float& axis_y, float deadzone);

    void radial(float& axis_x, float& axis_y, float deadzone);

    void smooth(float& axis, float deadzone);

    void smooth(float& axis_x, float& axis_y, float deadzone);
} // vn::deadzone
