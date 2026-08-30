#pragma once

#include <glm/glm.hpp>

#include "vinter/ecs.hpp"

namespace vn::component {
    struct Transform {
        struct Data {
            glm::vec2 position {};
            glm::vec2 scale { 1.f, 1.f };
            float rotation {};

            friend Data operator*(const Data& parent, const Data& local) {
                Data result {};

                float sin = std::sin(parent.rotation);
                float cos = std::cos(parent.rotation);

                glm::vec2 scaled = local.position * parent.scale;
                glm::vec2 rotated = {
                    scaled.x * cos - scaled.y * sin,
                    scaled.x * sin + scaled.y * cos,
                };

                result.position = parent.position + rotated;
                result.scale = parent.scale * local.scale;
                result.rotation = parent.rotation + local.rotation;

                return result;
            }
        };

        Data local {};
        Data global {};
        vn::Entity parent {};
    };
} // namespace vn::component

namespace vn::system {
    void resolve_transform_tree(ECS& ecs);
} // namespace vn::system