#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_ALIAS_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_ALIAS_HPP

#include TUNUM_COMMON_INCLUDE(fmpint/core.hpp)

namespace tunum
{
    // -------------------------------------------
    // エイリアス定義
    // -------------------------------------------

    using int128_t = fmpint<(sizeof(std::uint64_t) << 1), true>;
    using int256_t = fmpint<(sizeof(std::uint64_t) << 2), true>;
    using int512_t = fmpint<(sizeof(std::uint64_t) << 3), true>;

    using uint128_t = fmpint<(sizeof(std::uint64_t) << 1), false>;
    using uint256_t = fmpint<(sizeof(std::uint64_t) << 2), false>;
    using uint512_t = fmpint<(sizeof(std::uint64_t) << 3), false>;
}

#endif
