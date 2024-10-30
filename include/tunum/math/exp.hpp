#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_EXP_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_EXP_HPP

#include TUNUM_COMMON_INCLUDE(concepts.hpp)

#include <bit>
#include <array>
#include <vector>
#include <numbers>
#include <limits>

namespace tunum::math_impl
{
    struct exp_cpo
    {
        using calc_uint_t = std::uint64_t;
        static constexpr int pre_max_power = std::numeric_limits<calc_uint_t>::digits;
        static constexpr auto max_pre_table_exp = calc_uint_t(1) << (pre_max_power - 1);

        // 2の累乗値による指数の計算済みテーブル
        // 整数がオバーフローしないような最大値まで計算
        static constexpr auto pre_table = []() {
            constexpr auto size = pre_max_power + 1;
            auto table = std::array<double, size>{};
            table[0] = 1;
            for (int i = 1; i < size; i++)
                table[i] = i == 1
                    ? std::numbers::e_v<double>
                    : table[i - 1] * table[i - 1];
            return table;
        }();

        // 計算済みテーブルを用いて、整数値の指数関数を算出
        static constexpr double exp_uint(calc_uint_t x) noexcept
        {
            // 事前計算を行っていないサイズのxの場合は、計算済みとなるまで再帰
            if (std::bit_width(x) > pre_max_power)
                return pre_table[pre_max_power] * exp_uint(x - pre_max_power);

            double result = 1;
            for (; x > 0; x -= std::bit_floor(x))
                result *= pre_table[std::bit_width(x)];
            return result;
        }

        // マクローリン展開による近似
        static constexpr auto exp_maclaurin(std::floating_point auto x) noexcept
        {
            double a = 1;
            double total = 1;
            for (int i = 1; true; i++) {
                const auto before_total = total;
                a *= x / i;
                total += a;
                if (before_total == total)
                    break;
            }
            return total;
        }

        // // 整数変換時のオーバーフロー回避
        // template <std::floating_point FloatT>
        // static constexpr FloatT exp_avoid_uint_overflow(FloatT x, calc_uint_t n = 0) noexcept
        // {
        //     if (!n) {
        //         // xがオバーフローしない整数最大値の何乗か計算
        //         calc_uint_t pow_n = 1;
        //         for (
        //             auto pow = static_cast<FloatT>(max_pre_table_exp);
        //             x > pow;
        //             pow *= pow, pow_n <<= 1
        //         );
        //         return exp_avoid_uint_overflow(x, pow_n);
        //     }

        //     for (; n > 0; n >>= 1) {
                
        //     }
        // }

        // 実装
        template <std::floating_point FloatT>
        static constexpr FloatT impl(FloatT x) noexcept
        {
            // 計算テーブルの圧縮のため負の値は逆数に委譲
            if (x < 0)
                return 1. / impl<FloatT>(-x);

            // // 整数変換時のオーバーフロー回避
            // if (x > max_pre_table_exp) {
            //     constexpr auto 
            // }

            // 整数部分は計算テーブルから引き、本関数では 0 < x < 1 の範囲のみ近似値の計算実施
            const auto x_uint = static_cast<calc_uint_t>(x);
            return (x_uint > 0)
                ? exp_uint(x_uint) * impl<FloatT>(x - x_uint)
                : exp_maclaurin(x);
        }

        constexpr auto operator()(std::floating_point auto x) const noexcept
        { return impl(x); }

        // 任意の実装用オーバーロード
        template <class T>
        constexpr T operator()(T x) const
        { return exp(x); }
    };
}

namespace tunum
{
    // 指数関数の近似値算出。
    // 参考: https://qiita.com/MilkySaitou/items/614fcbb110cae5b9f797
    // @param x 求めたい指数(浮動小数点型)
    inline constexpr math_impl::exp_cpo exp{};
}

#endif