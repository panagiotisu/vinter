#pragma once

#include "vinter/assets/handle.hpp"

namespace vn::ecs {
    template <typename... Components>
    class Query;
    class Database;

    class Entity : public Handle {
        friend class Database;

        template <typename... Components>
        friend class Query;

        using Handle::Handle;
    };
} // namespace vn::ecs