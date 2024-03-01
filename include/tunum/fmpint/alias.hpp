#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_ALIAS_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_ALIAS_HPP

#include TUNUM_COMMON_INCLUDE(fmpint/core.hpp)

namespace tunum
{
    // -------------------------------------------
    // エイリアス定義
    // -------------------------------------------

    using int128_t = fmpint<(sizeof(std::uint64_t) << 1)>;
    using int256_t = fmpint<(sizeof(std::uint64_t) << 2)>;
    using int512_t = fmpint<(sizeof(std::uint64_t) << 3)>;
}

#endif
