#pragma once

#include <string>

namespace vn {
#if defined(__clang__) || defined(__GNUC__)
    #include <cstdlib>
    #include <cxxabi.h>

    // Returns a human-readable type name; falls back to the mangled name if demangling fails.
    inline std::string demangle(const char* mangled) {
        int status;
        char* buf = abi::__cxa_demangle(mangled, nullptr, nullptr, &status);
        std::string result = (status == 0 && buf) ? buf : mangled;
        std::free(buf);
        return result;
    }
#else
    inline std::string demangle(const char* mangled) {
        return mangled;
    }
#endif
} // namespace vn
