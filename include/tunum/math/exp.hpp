#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_EXP_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_EXP_HPP

#include TUNUM_COMMON_INCLUDE(concepts.hpp)
#include TUNUM_COMMON_INCLUDE(floating.hpp)

#include <bit>
#include <cmath>
#include <array>
#include <numbers>
#include <limits>

namespace tunum::_math_impl
{
    // 組み込み浮動小数点型の独自実装
    // @note 参考: https://qiita.com/MilkySaitou/items/614fcbb110cae5b9f797
    // TODO: stdfloatにも対応が必要か？
    struct std_floating_exp_impl
    {
        // 下記以降のインデックスの計算済みテーブルはlong double型で無限大となる最大指数までしか計算しない
        static constexpr int pre_max_power = std::bit_width((std::uint64_t)std::numeric_limits<long double>::max_exponent) - 1;
        // 10以降のインデックスの計算済みテーブルは無限大となるため、計算しない
        static constexpr auto pre_table_size = pre_max_power + 1;

        // 2の累乗値による指数の計算済みテーブル
        static constexpr auto pre_table = [] {
            auto table = std::array<long double, pre_table_size>{};
            table[0] = 1;
            auto value = std::numbers::e_v<long double>;
            for (int i = 1; i < pre_table_size; i++)
                table[i] = i == 1
                    ? value
                    : (value *= value);
            return table;
        }();

        // 整数部分
        template <std::floating_point FloatT>
        static constexpr std::array<long double, 2> exp_uint(FloatT x) noexcept
        {
            long double result = 1;
            for (int i = pre_max_power; i > 0; i--)
                for (const auto power_of_2 = 1ull << (i - 1); x >= power_of_2; x -= power_of_2)
                    result *= pre_table[i];
            return {x, result};
        }

        // マクローリン展開による近似
        static constexpr auto exp_maclaurin(std::floating_point auto x) noexcept
        {
            long double a = 1;
            long double total = 1;
            for (int i = 1; true; i++) {
                const auto before_total = total;
                a *= x / i;
                total += a;
                if (before_total == total)
                    break;
            }
            return total;
        }

        // 実装
        template <std::floating_point FloatT>
        static constexpr FloatT run(FloatT x) noexcept
        {
            // 計算テーブルの圧縮のため負の値は逆数に委譲
            if (x < 0)
                return 1. / run<FloatT>(-x);

            // x > 2 ^ 10 の場合、long double型は無限大となるので、最大値返却
            if (x >= (2ull << pre_max_power))
                return pre_table.back();
                
            // 整数部分は計算テーブルから引き、本関数では 0 < x < 1 の範囲のみ近似値の計算実施
            if (x >= 1) {
                const auto [uint_result_x, uint_result_value] = exp_uint(x);
                return uint_result_value * exp_maclaurin(uint_result_x);
            }
            return exp_maclaurin(x);
        }
    };

    // -----------------------------------------
    // 指数関数のオーバーロード列挙および、cpo定義
    // -----------------------------------------

    template <std::floating_point FloatT>
    inline constexpr auto exp(FloatT x) noexcept
    {
        // 標準ライブラリの指数関数がコンパイル時評価可能だったら常に標準ライブラリの実装を使う
        if constexpr (is_constexpr([] { auto val = std::exp(FloatT{}); }))
            return std::exp(x);
        else {
            if (!std::is_constant_evaluated())
                return std::exp(x);
            return std_floating_exp_impl::run(x);
        }
    }

    struct exp_cpo
    {
        constexpr auto operator()(auto x) const noexcept
        { return exp(x); }
    };
}

namespace tunum
{
    // 指数関数の近似値算出。
    // @param x 求めたい指数
    inline constexpr _math_impl::exp_cpo exp{};
}

#endif
