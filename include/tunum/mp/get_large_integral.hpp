#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MP_GET_LARGE_INTEGRAL_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MP_GET_LARGE_INTEGRAL_HPP

#include TUNUM_COMMON_INCLUDE(submodule_loader.hpp)
#include TUNUM_COMMON_INCLUDE(mp/is_unsigned_fmpint.hpp)
#include TUNUM_COMMON_INCLUDE(mp/arithmetc_operation_result.hpp)

namespace tunum::tpfn
{
    // 制約 TuIntegral が真となる二つの型のうち、大きいほうと同じサイズの fmpint を返却
    // @tparam T1
    // @tparam T2
    // @tparam T1_or_T2 サイズが等しくなった時、適用させたい符号の型を指定
    template <class T1, class T2, class T1_or_T2>
    requires (tunum::is_integral_v<T1>
        && tunum::is_integral_v<T2>
        && tunum::is_integral_v<T1_or_T2>
    )
    struct get_large_integral : public std::type_identity<
        fmpint<
            get_integral_size_v<arithmetc_operation_result_t<T1, T2>>,
            !tunum::is_unsigned_v<T1_or_T2>
        >
    > {};
}

namespace tunum
{
    // 制約 TuIntegral が真となる二つの型のうち、大きいほうと同じサイズの fmpint を返却
    // @tparam T1
    // @tparam T2
    // @tparam T1_or_T2 サイズが等しくなった時、適用させたい符号の型を指定
    using get_large_integral = tump::cbk<tpfn::get_large_integral, 3>;

    // 制約 TuIntegral が真となる二つの型のうち、大きいほうと同じサイズの fmpint を返却
    // @tparam T1
    // @tparam T2
    // @tparam T1_or_T2 サイズが等しくなった時、適用させたい符号の型を指定
    template <class T1, class T2, class T1_or_T2>
    using get_large_integral_t = typename tpfn::get_large_integral<T1, T2, T1_or_T2>::type;
}

#endif
