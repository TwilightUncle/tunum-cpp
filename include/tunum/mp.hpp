// ---------------------------------------
// メタプログラミング
// ---------------------------------------
#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MP_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MP_HPP

#ifndef TUNUM_COMMON_INCLUDE
#define TUNUM_COMMON_INCLUDE(path) <tunum/path>
#endif

#include TUNUM_COMMON_INCLUDE(mp/integral_to_floating.hpp)
#include TUNUM_COMMON_INCLUDE(mp/is_unsigned_fmpint.hpp)
#include TUNUM_COMMON_INCLUDE(mp/get_int.hpp)
#include TUNUM_COMMON_INCLUDE(mp/arithmetc_operation_result.hpp)

namespace tunum
{
    // 制約 TuIntegral が真となる二つの型のうち、大きいほうと同じサイズの fmpint を返却
    template <class T1, class T2, class T1_or_T2>
    requires ((is_fmpint_v<T1> || std::is_integral_v<T1>)
        && (is_fmpint_v<T2> || std::is_integral_v<T2>)
        && (is_fmpint_v<T1_or_T2> || std::is_integral_v<T1_or_T2>)
    )
    struct get_large_integral : public std::type_identity<
        fmpint<(std::max)(sizeof(T1), sizeof(T2)), !(is_unsigned_fmpint_v<T1_or_T2> || std::is_unsigned_v<T1_or_T2>)>
    > {};

    // 制約 TuIntegral が真となる二つの型のうち、大きいほうと同じサイズの fmpint を返却
    template <class T1, class T2, class T1_or_T2>
    using get_large_integral_t = typename get_large_integral<T1, T2, T1_or_T2>::type;
}

#endif
