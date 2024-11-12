#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_ADD_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_ADD_HPP

#include TUNUM_COMMON_INCLUDE(floating/fe_fn.hpp)

namespace tunum
{
    // 加算(減算の実装も兼ねる)
    template <std::floating_point Arg1, std::floating_point Arg2>
    struct add : public fe_fn<Arg1, Arg2>
    {
        using parent_t = fe_fn<Arg1, Arg2>;
        using calc_t = typename parent_t::calc_t;
        using info_t = typename parent_t::info_t;
        using validate_result_t = typename parent_t::validate_result_t;

        // -------------------------------------
        // コンストラクタ
        // -------------------------------------

        constexpr add(const fe_holder<Arg1>& arg1, const fe_holder<Arg2>& arg2)
            : parent_t(fn, validate, check_result, arg1, arg2)
        {}

        // ---------------------------------------
        // 処理・検証の実装
        // ---------------------------------------

        static constexpr auto fn = [](calc_t arg1, calc_t arg2)
        { return arg1 + arg2; };

        static constexpr auto validate = [](const info_t& info1, const info_t& info2) -> validate_result_t
        {
            const auto [e, is_run, return_value] = parent_t::validate_arg_default(info1, info2);
            if (!is_run)
                return {e, is_run, return_value};

            const auto num_of_infinity = parent_t::count_infinity(info1, info2);
            const auto is_same_sign = info1.sign() == info2.sign();
            // 両方無限大かつ符号が異なる場合は不正な演算とする
            if (num_of_infinity == 2 && !is_same_sign)
                return {FE_INVALID, false, info_t::get_nan()};
            // 少なくともどちらか一方が無限大(データが失われたわけではないので、エラーはなし)
            if (num_of_infinity > 0)
                return {
                    std::fexcept_t{},
                    false,
                    parent_t::get_first_infinity(info1, info2)
                };

            // 合計値が無限大になるか予測(符号が異なる場合は発生しえない)
            // ※gccで定数式の無限大発生が許可されていないようなので、あらかじめオーバーフローが発生するか予測する
            if (is_same_sign) {
                // まず、正規化数最大値の情報落ち最大値よりも両方のデータが大きくなければいけない
                // さらに、正規化数の最大値より一方の値を引いた結果より、もう一方の値が大きい場合に無限大となる
                constexpr auto norm_max = info_t::make_max();
                constexpr auto lost_max = norm_max.make_lost_info_max();
                const auto sign = info1.sign();
                const auto cond1 = (calc_t)lost_max < (calc_t)info1.change_sign(false)
                    && (calc_t)lost_max < (calc_t)info2.change_sign(false);
                const auto cond2 = (calc_t)norm_max - (calc_t)info1.change_sign(false) < (calc_t)info2.change_sign(false);
                if (cond1 && cond2)
                    return {FE_INEXACT | FE_OVERFLOW, false, info_t::get_infinity(sign < 0)};
            }
            return {std::fexcept_t{}, true, calc_t{}};
        };

        static constexpr auto check_result = [](const info_t& result, const info_t& arg1, const info_t& arg2) -> std::fexcept_t
        {
            // // 有限数と有限数の加算から無限が生じているのでオーバーフロー
            // if (result.is_infinity())
            //     return FE_INEXACT | FE_OVERFLOW;

            // 結果がゼロなのに、符号をそろえたオペランドが異なる場合アンダーフロー
            //  ->発生しうるの？
            return (result.is_zero() && (calc_t)arg1 != -(calc_t)arg2)
                ? FE_INEXACT | FE_UNDERFLOW
                : parent_t::check_after_default(result, arg1, arg2);
        };
    };
}

#endif
