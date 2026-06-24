#pragma once

#include <cstdint>
#include <limits>

namespace vn {
    class Entity {
    public:
        // Entity::Id uses the first bits for versioning, and any subsequent bits for the actual
        // entity Id index.
        // Entity::Id = [ Entity::Version | Entity::Index ]
        using Id = std::uint64_t;
        using Version = std::uint16_t;
        using Index = std::uint64_t;

        static constexpr std::uint32_t IdBits {sizeof(Id) * 8};
        static constexpr std::uint32_t VersionBits {12};
        static constexpr std::uint32_t IndexBits = IdBits - VersionBits;

        static constexpr Id IndexMask {(static_cast<Id>(1) << IndexBits) - 1};
        static constexpr Id VersionMask {~IndexMask};
        static constexpr Version MaxVersion {(static_cast<Version>(1) << VersionBits) - 1};
        static constexpr Id Null {std::numeric_limits<Id>::max()};

        [[nodiscard]]
        auto id() const noexcept -> Id {
            return m_id;
        };

        [[nodiscard]]
        auto version() const noexcept -> Version {
            return static_cast<Version>((m_id & VersionMask) >> IndexBits);
        }

        [[nodiscard]]
        auto index() const noexcept -> Index {
            return static_cast<Index>(m_id & IndexMask);
        }

        [[nodiscard]]
        auto is_null() const noexcept -> bool {
            return m_id == Null;
        }

        constexpr explicit operator bool() const {
            return !is_null();
        }

        friend constexpr auto operator==(Entity lhs, Entity rhs) -> bool {
            return lhs.m_id == rhs.m_id;
        }

        friend constexpr auto operator!=(Entity lhs, Entity rhs) -> bool {
            return !(lhs == rhs);
        }

    private:
        Id m_id {};

        Entity(Version version, Index index) : m_id(encode(version, index)) {
            static_assert(IdBits == sizeof(Id) * 8);
            static_assert(VersionBits <= sizeof(Version) * 8);
            static_assert(IndexBits <= sizeof(Index) * 8);
        }

        static constexpr auto encode(Version version, Index index) -> Id {
            return ((static_cast<Id>(version) << IndexBits) & VersionMask)
                   | (static_cast<Id>(index) * IndexMask);
        }
    };
} // namespace vn