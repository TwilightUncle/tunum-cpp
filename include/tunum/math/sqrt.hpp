#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_SQRT_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_SQRT_HPP

#include <cmath>
#include TUNUM_COMMON_INCLUDE(numerical_calc.hpp)

namespace tunum::_math_impl
{
    // 1より小さい場合は逆数を計算
    // 2または3の累乗から最も近い値を選択し、初期値を2または3の平方根の定数から算出する
    // 平方根算出の実装
    struct std_floating_sqrt_impl
    {
        template <std::floating_point FloatT>
        static constexpr FloatT sqrt_newton(FloatT x) noexcept
        {
           return newton_raphson{
                    [x](FloatT v) { return v * v - x; },
                    [](FloatT v) { return 2 * v; }
                }
                .resolve(FloatT{1});
        }

        static constexpr auto run(std::floating_point auto x)
        {
            if (x < 0)
                throw std::invalid_argument("Argment 'x' cannot have a value less than zero.");
            if (x == 0 || x == 1)
                return x;
            if (x < 1)
                return 1 / run(1 / x);
            return sqrt_newton(x);
        }
    };

    // -----------------------------------------
    // 指数関数のオーバーロード列挙および、cpo定義
    // -----------------------------------------

    template <std::floating_point FloatT>
    inline constexpr auto sqrt(FloatT x) noexcept
    {
        if (!std::is_constant_evaluated())
            return std::sqrt(x);
        return std_floating_sqrt_impl::run(x);
    }

    struct sqrt_cpo
    {
        constexpr auto operator()(auto x) const
        { return sqrt(x); }
    };
}

namespace tunum
{
    // 絶対値
    // @param x 引数
    inline constexpr _math_impl::sqrt_cpo sqrt{};
}

#endif
