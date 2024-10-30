#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_EXP_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_EXP_HPP

#include TUNUM_COMMON_INCLUDE(concepts.hpp)

#include <array>
#include <numbers>
#include <stdexcept>

namespace tunum::math_impl
{
    struct exp_cpo
    {
    private:
        // あらかじめ、eの整数乗を計算し、テーブルを生成しておく
        template <int max_power>
        static constexpr auto make_table()
        {
            constexpr int size = max_power + 1;
            constexpr int minor_max_power = max_power >> 1;
            if constexpr (max_power == 1)
                return std::array<double, size>{ 1., std::numbers::e_v<double> };
            else {
                constexpr auto minor_table = make_table<minor_max_power>();
                auto table = std::array<double, size>{};
                for (int i = 0; i <= minor_max_power; i++) {
                    table[i] = minor_table[i];
                    table[i + minor_max_power] = minor_table[i] * minor_table.back();
                }
                return table;
            }
        }

        // TODO: 設定ファイルみたいなところでdefineできるようにする
        static constexpr int pre_max_power = 64;

        // 整数の計算済みテーブル
        static constexpr auto pre_table = make_table<pre_max_power>();

        // 実装
        template <std::floating_point FloatT>
        constexpr FloatT impl(FloatT x) const
        {
            // 計算テーブルの圧縮のため負の値は逆数に委譲
            if (x < 0)
                return 1. / impl<FloatT>(-x);

            // 整数部分は計算テーブルから引く
            const auto x_integral = pre_max_power > x
                ? static_cast<int>(x)
                : pre_max_power;
            if (x_integral > 0)
                return pre_table[x_integral] * impl<FloatT>(x - x_integral);

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

    public:
        template <std::floating_point FloatT>
        constexpr FloatT operator()(FloatT x) const
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