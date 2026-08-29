#pragma once

#include <glm/glm.hpp>

namespace vn {
    struct Line {
        glm::vec2 start {};
        glm::vec2 end {};
    };

    struct AABB {
        glm::vec2 position {};
        glm::vec2 size {};
    };

    struct Circle {
        glm::vec2 center {};
        float radius {};
    };
} // namespace vn