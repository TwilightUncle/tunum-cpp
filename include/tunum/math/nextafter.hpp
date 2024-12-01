#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_NEXTAFTER_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_NEXTAFTER_HPP

#include <cmath>
#include TUNUM_COMMON_INCLUDE(floating.hpp)

namespace tunum::_math_impl
{
    // --------------------------------------
    // fe_holder向けの実装
    // --------------------------------------

    template <std::floating_point T1, std::floating_point T2, std::fexcept_t RaiseFeFlags>
    struct fe_nextafter : public fe_fn<RaiseFeFlags, T1, T2>
    {
        using parent_t = fe_fn<RaiseFeFlags, T1, T2>;
        using calc_t = typename parent_t::calc_t;
        using info_t = typename parent_t::info_t;
        using validate_result_t = typename parent_t::validate_result_t;

        // -------------------------------------
        // コンストラクタ
        // -------------------------------------

        constexpr fe_nextafter(
            const fe_holder<T1, RaiseFeFlags>& arg1,
            const fe_holder<T2, RaiseFeFlags>& arg2
        )
            : parent_t(fn, validate, check_result, arg1, arg2)
        {}

        // ---------------------------------------
        // 処理・検証の実装
        // ---------------------------------------

        static constexpr auto fn = [](calc_t arg1, calc_t arg2)
        { return info_t{arg1}.nextafter(arg2); };

        static constexpr auto validate = [](const info_t& info1, const info_t& info2) -> validate_result_t
        {
            const auto [e, is_run, return_value] = parent_t::validate_arg_default(info1, info2);

            // 実装がbit操作による値の生成なので、
            // オーバーフローによる例外でコンパイルエラーとはならないため、
            // この時点での予測は不要。
            // 結果を見て例外の設定を行う
            return {e, is_run, return_value};
        };

        static constexpr auto check_result = [](const info_t& result, const info_t& arg1, const info_t& arg2) -> std::fexcept_t
        {
            if (arg1 == arg2)
                return std::fexcept_t{};
            // アンダーフロー
            if (result.is_denormalized() || result.is_zero())
                return FE_INEXACT | FE_UNDERFLOW;
            // オーバーフロー
            return result.is_infinity()
                ? FE_INEXACT | FE_OVERFLOW
                : std::fexcept_t{};
        };
    };

    // --------------------------------------
    // fe_nextafterの推論補助
    // --------------------------------------

    template <class Arg1, std::floating_point Arg2, std::fexcept_t RaiseFeFlags>
    requires (std::is_arithmetic_v<Arg1>)
    fe_nextafter(Arg1, fe_holder<Arg2, RaiseFeFlags>)
        -> fe_nextafter<integral_to_floating_t<Arg1, double>, Arg2, RaiseFeFlags>;

    template <std::floating_point Arg1, class Arg2, std::fexcept_t RaiseFeFlags>
    requires (std::is_arithmetic_v<Arg2>)
    fe_nextafter(fe_holder<Arg1, RaiseFeFlags>, Arg2)
        -> fe_nextafter<Arg1, integral_to_floating_t<Arg2, double>, RaiseFeFlags>;

    template <class Arg1, class Arg2>
    requires (std::is_arithmetic_v<Arg1> && std::is_arithmetic_v<Arg2>)
    fe_nextafter(Arg1, Arg2)
        -> fe_nextafter<integral_to_floating_t<Arg1, double>, integral_to_floating_t<Arg2, double>, std::fexcept_t{}>;

    // --------------------------------------
    // オーバーロード
    // --------------------------------------

    // 組み込みの実装用
    template <class T1, class T2>
    requires (std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>)
    inline constexpr auto nextafter(T1 x, T2 y)
    {
        // 戻り値の統一のため、整数値はdoubleに変換
        using calc_t1 = integral_to_floating_t<T1, double>;
        using calc_t2 = integral_to_floating_t<T2, double>;
        if (!std::is_constant_evaluated())
            return std::nextafter(calc_t1(x), calc_t2(y));
        return fe_nextafter{calc_t2(x), calc_t2(y)}.value;
    }

    // inline constexpr double nextafter(T x, T y)
    // { return nextafter(double(x), double(y)); }

    // どちらかの引数がfe_holder
    template <class T1, class T2>
    requires (is_fe_holder_v<T1> || is_fe_holder_v<T2>)
    inline constexpr auto nextafter(T1 x, T2 y)
    { return fe_nextafter{x, y}; }

    // --------------------------------------
    // cpo
    // --------------------------------------

    struct nextafter_cpo
    {
        constexpr auto operator()(auto x, auto y) const
        { return nextafter(x, y); }
    };
}

namespace tunum
{
    // xから見た、yの方向へ存在する隣接値を取得
    // x == yの場合は、値が変化しない
    // @param x
    // @param y
    inline constexpr _math_impl::nextafter_cpo nextafter{};
}

#endif
