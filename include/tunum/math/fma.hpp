#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_FMP_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_FMP_HPP

#include TUNUM_COMMON_INCLUDE(floating.hpp)

namespace tunum::_math_impl
{
    // --------------------------------------
    // fma向けコンセプト
    // --------------------------------------

    // 標準ライブラリのfma関数が定数式場で呼び出し可能か
    template <class T1, class T2, class T3>
    concept StdFmaConstantCallable =  requires {
        typename require_constant<std::fma(std::declval<T1>(), std::declval<T2>(), std::declval<T3>())>;
    };

    // --------------------------------------
    // 組み込み型向けのオーバーロード定義
    // --------------------------------------

    template <class T1, class T2, class T3>
    requires (
        tump::count_if<tump::is_arithmetic, tump::list<T1, T2, T3>> == 3
    )
    inline constexpr auto fma(T1 x, T2 y, T3 z) noexcept
    {
        // 戻り値型を明示的に記述しておく
        using calc_t = integral_to_floating_t<math_calclation_result_t<T1, T2, T3>>;
        if (!std::is_constant_evaluated() || StdFmaConstantCallable<T1, T2, T3>)
            return calc_t(std::fma(x, y, z));
        return calc_t(x) * calc_t(y) + calc_t(z);
    }

    // --------------------------------------
    // fe_holder向けの実装
    // --------------------------------------

    template <std::floating_point T1, std::floating_point T2, std::floating_point T3, std::fexcept_t RaiseFeFlags>
    struct fe_fma : public fe_fn<RaiseFeFlags, T1, T2, T3>
    {
        using parent_t = fe_fn<RaiseFeFlags, T1, T2, T3>;
        using calc_t = typename parent_t::calc_t;
        using info_t = typename parent_t::info_t;
        using validate_result_t = typename parent_t::validate_result_t;
        using mul_t = mul<calc_t, calc_t, RaiseFeFlags>;
        using add_t = add<calc_t, calc_t, RaiseFeFlags>;

        // -------------------------------------
        // コンストラクタ
        // -------------------------------------

        constexpr fe_fma(
            const fe_holder<T1, RaiseFeFlags>& arg1,
            const fe_holder<T2, RaiseFeFlags>& arg2,
            const fe_holder<T3, RaiseFeFlags>& arg3
        )
            : parent_t(fn, validate, check_result, arg1, arg2, arg3)
        {}

        // ---------------------------------------
        // 処理・検証の実装
        // ---------------------------------------

        static constexpr auto fn = [](calc_t arg1, calc_t arg2, calc_t arg3)
        { return fma(arg1, arg2, arg3); };

        static constexpr auto validate = [](const info_t& info1, const info_t& info2, const info_t& info3) -> validate_result_t
        {
            const auto [e, is_run, return_value] = parent_t::validate_arg_default(info1, info2);
            if (!is_run)
                return {e, is_run, return_value};
            const auto [e1, is_run1, return_val1] = mul_t::validate(info1, info2);
            if (!is_run1)
                return {e | e1, is_run1, return_val1};
            const auto [e2, is_run2, return_val2] = add_t::validate(info_t{calc_t(info1) * calc_t(info2)}, info3);
            return {e | e1 | e2, is_run2, return_val2};
        };

        static constexpr auto check_result = [](const info_t& result, const info_t& arg1, const info_t& arg2, const info_t& arg3) -> std::fexcept_t
        {
            return mul_t::check_result(result, arg1, arg2)
                | add_t::check_result(result, info_t{calc_t(arg1) * calc_t(arg2)}, arg3);
        };
    };

    // --------------------------------------
    // fe_fmaの推論補助
    // --------------------------------------

    template <std::floating_point T1, class T2, class T3, std::fexcept_t RaiseFeFlags>
    requires (std::is_arithmetic_v<T2> && std::is_arithmetic<T3>)
    fe_fma(fe_holder<T1, RaiseFeFlags>, T2, T3)
        -> fe_fma<T1, integral_to_floating_t<T2, T1>, integral_to_floating_t<T3, T1>, RaiseFeFlags>;

    template <class T1, std::floating_point T2, class T3, std::fexcept_t RaiseFeFlags>
    requires (std::is_arithmetic<T3>)
    fe_fma(T1, fe_holder<T2, RaiseFeFlags>, T3)
        -> fe_fma<integral_to_floating_t<T1, T2>, T2, integral_to_floating_t<T3, T2>, RaiseFeFlags>;

    template <class T1, class T2, std::floating_point T3, std::fexcept_t RaiseFeFlags>
    fe_fma(T1, T2, fe_holder<T3, RaiseFeFlags>)
        -> fe_fma<integral_to_floating_t<T1, T3>, integral_to_floating_t<T2, T3>, T3, RaiseFeFlags>;

    template <class T1, class T2, class T3>
    requires (tump::count_if<tump::is_fe_holder, tump::list<T1, T2, T3>> == 3)
    fe_fma(T1, T2, T3)
        -> fe_fma<integral_to_floating_t<T1, double>, integral_to_floating_t<T2, double>, integral_to_floating_t<T3, double>, std::fexcept_t{}>

    // --------------------------------------
    // fe_holder向けのオーバーロード
    // --------------------------------------

    // fe_holderを含む場合のオーバーロード
    // fmaによる高速化の恩恵はないが、
    // fe_holderを使用しない場合との移植性保持のため定義しておく
    template <class T1, class T2, class T3>
    requires (
        tump::count_if<tump::is_fe_holder, tump::list<T1, T2, T3>> != 0
        && tump::count_if<tump::is_fe_holder, tump::list<T1, T2, T3>>
        +  tump::count_if<tump::is_arithmetic, tump::list<T1, T2, T3>>
        == 3
    )
    inline constexpr auto fma(T1 x, T2 y, T3 z) noexcept
    { return fe_fma{x, y, z}; }

    // --------------------------------------
    // cpo
    // --------------------------------------

    struct fma_cpo
    {
        constexpr auto operator()(auto x, auto y, auto z) const
        { return fma(x, y, z); }
    };
}

namespace tunum
{
    // fused multiply-add
    // @param x 引数
    inline constexpr _math_impl::fma_cpo fma{};
}

#endif
