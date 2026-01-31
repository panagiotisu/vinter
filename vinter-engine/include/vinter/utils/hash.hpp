#pragma once

#include <cstdint>
#include <string_view>

namespace vn {
    [[nodiscard]] constexpr std::uint64_t fnv1a_64(const std::string_view str) noexcept {
        std::uint64_t hash = 0xcbf29ce484222325ull;  // FNV-1a 64-bit offset basis
        for (const unsigned char c : str) {
            hash ^= c;
            hash *= 0x100000001b3ull;  // FNV-1a 64-bit prime
        }
        return hash;
    }
} // vn