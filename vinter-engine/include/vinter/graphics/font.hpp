#pragma once

#include "vinter/assets/handle.hpp"

namespace vn {
    class Font : public Handle {
        friend class FontManager;
        using Handle::Handle;
    };
} // namespace vn