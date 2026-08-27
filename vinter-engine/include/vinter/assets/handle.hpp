#pragma once

#include <cstdint>
#include <limits>

namespace vn {
    class Handle {
    public:
        // Handle::Id uses the first bits for versioning, and any subsequent bits for the actual
        // handle Id index.
        // Handle::Id = [ Handle::Version | Handle::Index ]
        using Id = std::uint64_t;
        using Version = std::uint16_t;
        using Index = std::uint64_t;

        static constexpr std::uint32_t IdBits { sizeof(Id) * 8 };
        static constexpr std::uint32_t VersionBits { 12 };
        static constexpr std::uint32_t IndexBits { IdBits - VersionBits };

        static constexpr Id IndexMask { (static_cast<Id>(1) << IndexBits) - 1 };
        static constexpr Id VersionMask { ~IndexMask };
        static constexpr Version MaxVersion { (static_cast<Version>(1) << VersionBits) - 1 };
        static constexpr Id Null { std::numeric_limits<Id>::max() };

    public:
        Handle() = default;

        [[nodiscard]]
        Id id() const noexcept {
            return m_id;
        };

        [[nodiscard]]
        Version version() const noexcept {
            return static_cast<Version>((m_id & VersionMask) >> IndexBits);
        }

        [[nodiscard]]
        Index index() const noexcept {
            return static_cast<Index>(m_id & IndexMask);
        }

        [[nodiscard]]
        constexpr bool is_null() const noexcept {
            return m_id == Null;
        }

        constexpr explicit operator bool() const {
            return !is_null();
        }

        friend constexpr bool operator==(Handle lhs, Handle rhs) {
            return lhs.m_id == rhs.m_id;
        }

        friend constexpr bool operator!=(Handle lhs, Handle rhs) {
            return !(lhs == rhs);
        }

    protected:
        Id m_id { Null };

        Handle(Version version, Index index) : m_id(encode(version, index)) {
            static_assert(IdBits == sizeof(Id) * 8);
            static_assert(VersionBits <= sizeof(Version) * 8);
            static_assert(IndexBits <= sizeof(Index) * 8);
        }

        static constexpr Id encode(Version version, Index index) {
            return ((static_cast<Id>(version) << IndexBits) & VersionMask)
                   | (static_cast<Id>(index) & IndexMask);
        }
    };
} // namespace vn