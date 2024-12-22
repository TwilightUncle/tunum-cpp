#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_SQRT_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_SQRT_HPP

#include <cmath>
#include TUNUM_COMMON_INCLUDE(numerical_calc.hpp)

namespace tunum::_math_impl
{
    // --------------------------------------
    // fe_holder向けの実装
    // --------------------------------------
    
    template <std::floating_point T, std::fexcept_t RaiseFeFlags>
    struct fe_sqrt : public fe_holder<T, RaiseFeFlags>
    {
        using info_t = floating_std_info<T>;
        using parent_t = fe_holder<T, RaiseFeFlags>;

        // -------------------------------------
        // コンストラクタ
        // -------------------------------------

        constexpr fe_sqrt(const parent_t& arg)
            : parent_t(run(arg))
        {}

        // ---------------------------------------
        // コア部分
        // ---------------------------------------

        static constexpr parent_t sqrt_newton(const parent_t& x) noexcept
        {
           return newton_raphson{
                    // 浮動小数点例外は下記のxを含む演算にて伝播するため、
                    // resolveの指定は不要
                    [x](const parent_t& v) { return v * v - x; },
                    [](const parent_t& v) { return 2 * v; }
                }
                .resolve(parent_t{1});
        }

        // 1より小さい場合は逆数を計算
        // 2または3の累乗から最も近い値を選択し、初期値を2または3の平方根の定数から算出する
        // 平方根算出の実装
        static constexpr parent_t run(const parent_t& x)
        {
            // 0未満の定義域エラー
            if (x < 0.f)
                return parent_t(info_t::get_nan(), x.fexcepts | FE_INVALID);
            // 自明な結果
            if (x == 0.f || x == 1.f)
                return x;
            if (x < 1.f)
                return 1.f / run(1.f / x);
            return sqrt_newton(x);
        }
    };

    // --------------------------------------
    // 推論補助
    // --------------------------------------

    template <class T>
    requires (std::is_arithmetic_v<T>)
    fe_sqrt(T)
        -> fe_sqrt<integral_to_floating_t<T, double>, std::fexcept_t{}>;

    // -----------------------------------------
    // 指数関数のオーバーロード列挙および、cpo定義
    // -----------------------------------------

    template <std::floating_point FloatT>
    inline constexpr auto sqrt(FloatT x) noexcept
    {
        if (!std::is_constant_evaluated())
            return std::sqrt(x);
        return fe_sqrt(x).value;
    }

    inline constexpr auto sqrt(const FeHoldable auto& x) noexcept
    { return fe_sqrt(x); }

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
