#pragma once

#include <tuple>

namespace vn {
    template <class... Types>
    struct TypeList {
        using TypeTuple = std::tuple<Types...>;

        template <std::size_t Index>
        using get = std::tuple_element_t<Index, TypeTuple>;

        static constexpr std::size_t size = sizeof...(Types);
    };
} // namespace vn