#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_APS_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_APS_HPP

#include <complex>
#include TUNUM_COMMON_INCLUDE(math/floating.hpp)

namespace tunum::_math_impl
{
    // --------------------------------------
    // オーバーロード
    // --------------------------------------

    // 組み込みの整数用
    template <std::integral T>
    requires (!std::unsigned_integral<T>)
    inline constexpr T abs(T x) noexcept
    {
        // 該当整数の最小値
        constexpr auto max_bit_pos = (sizeof(T) << 3) - 1;
        constexpr auto min_value = T{1} << max_bit_pos;
        // 最小値の絶対値は表現不能で未定義だが、
        // とりあえず来た値をそのまま返却
        return min_value == x
            ? T{x}
            : T{(std::max)(x, -x)};
    }

    // fmpint用
    template <TuFmpIntegral T>
    requires (!TuFmpUnsigned<T>)
    inline constexpr T abs(const T& x) noexcept
    {
        // 該当整数の最小値
        constexpr auto max_bit_pos = (sizeof(T) << 3) - 1;
        constexpr auto min_value = T{1} << max_bit_pos;
        // 最小値の絶対値は表現不能で未定義だが、
        // とりあえず来た値をそのまま返却
        return min_value == x
            ? T{x}
            : T{(std::max)(x, -x)};
    }

    // 組み込みの浮動小数点用
    inline constexpr auto abs(std::floating_point auto x) noexcept
    { return copysign(x, 1); }

    // fe_holder用
    inline constexpr auto abs(const FeHoldable auto& x) noexcept
    { return copysign(x, 1); }

    // // 絶対値取得(複素数用)
    // template <std::floating_point FloatT>
    // inline constexpr FloatT abs(const std::complex<FloatT>& x)
    // { return (std::max)(x, -x); }

    // --------------------------------------
    // cpo
    // --------------------------------------

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
