#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_DIV_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_DIV_HPP

#include TUNUM_COMMON_INCLUDE(floating/fe_fn.hpp)

namespace tunum
{
    template <std::floating_point Arg1, std::floating_point Arg2, std::fexcept_t RaiseFeFlags>
    struct div : public fe_fn<RaiseFeFlags, Arg1, Arg2>
    {
        using parent_t = fe_fn<RaiseFeFlags, Arg1, Arg2>;
        using calc_t = typename parent_t::calc_t;
        using info_t = typename parent_t::info_t;
        using validate_result_t = typename parent_t::validate_result_t;

        constexpr div(const fe_holder<Arg1, RaiseFeFlags>& arg1, const fe_holder<Arg2, RaiseFeFlags>& arg2)
            : parent_t(fn, validate, check_result, arg1, arg2)
        {}

        // ---------------------------------------
        // 処理・検証の実装
        // ---------------------------------------

        static constexpr auto fn = [](calc_t arg1, calc_t arg2)
        { return arg1 / arg2; };

        static constexpr auto validate = [](const info_t& info1, const info_t& info2) -> validate_result_t
        {
            const auto [e, is_run, return_value] = parent_t::validate_arg_default(info1, info2);
            if (!is_run)
                return {e, is_run, return_value};

            const auto small = (std::min)(info1.change_sign(false), info2.change_sign(false));
            const auto large = (std::max)(info1.change_sign(false), info2.change_sign(false));
            // どちらも無限またはどちらもゼロの時は不正
            if (small.is_infinity())
                return  {FE_INVALID, false, info_t::get_nan()};
            if (large.is_zero())
                return  {FE_INVALID | FE_DIVBYZERO, false, info_t::get_nan()};

            const auto is_minus = info1.sign() != info2.sign();
            // 第二引数が0の時は無限
            if (info2.is_zero())
                return {FE_DIVBYZERO, false, info_t::get_infinity(is_minus)};

            // 第一引数が無限の時は無限
            if (info1.is_infinity())
                return {std::fexcept_t{}, false, info_t::get_infinity(is_minus)};

            // 第一引数が0または、第二引数が無限の場合はゼロ
            if (info1.is_zero() || info2.is_infinity())
                return {std::fexcept_t{}, false, info_t::get_zero(is_minus)};

            // 指数部を0にした全引数を乗算した指数と両引数の指数部の合計値を合わせて、
            // 正規化数の最大値の指数部より大きければオーバーフロー
            constexpr auto norm_max = info_t::make_max();
            const auto info1_e0_d_info2_e0 = info_t{
                calc_t(info1.change_exponent(0)) / calc_t(info2.change_exponent(0))
            };
            return norm_max.exponent() < info1.exponent() - info2.exponent() + info1_e0_d_info2_e0.exponent()
                ? validate_result_t{FE_INEXACT | FE_OVERFLOW, false, info_t::get_infinity(is_minus)}
                : parent_t::make_validate_result_ok();
        };

        static constexpr auto check_result = [](const info_t& result, const info_t& arg1, const info_t& arg2) -> std::fexcept_t
        {
            // 演算前の検証時点で確認済み
            // // 有限数と有限数の除算から無限が生じているのでオーバーフロー
            // if (result.is_infinity())
            //     return FE_INEXACT | FE_OVERFLOW;

            // 結果がゼロなのに、どちらの引数もゼロではない
            // (片方がゼロの場合は、検証時点ではじかれる)
            return (result.is_denormalized() || result.is_zero())
                ? FE_INEXACT | FE_UNDERFLOW
                : std::fexcept_t{};
        };
    };
}

#endif
