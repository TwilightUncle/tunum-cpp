#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_APS_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_APS_HPP

#include <complex>
#include TUNUM_COMMON_INCLUDE(concepts.hpp)

namespace tunum::_math_impl
{
    // 絶対値取得(算術型用)
    inline constexpr auto abs(const TuArithmetic auto& x)
    { return (std::max)(x, -x); }

    // // 絶対値取得(複素数用)
    // template <std::floating_point FloatT>
    // inline constexpr FloatT abs(const std::complex<FloatT>& x)
    // { return (std::max)(x, -x); }

    struct abs_cpo
    {
        constexpr auto operator()(auto x) const
        { return abs(x); }
    };
}

namespace tunum
{
    // 絶対値
    // @param x 引数
    inline constexpr _math_impl::abs_cpo abs{};
}

#endif
