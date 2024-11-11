#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_ADD_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_ADD_HPP

#include TUNUM_COMMON_INCLUDE(floating/fe_fn.hpp)

namespace tunum
{
    // 加算
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
        constexpr add(Arg1 arg1, Arg2 arg2)
            : add(fe_holder{arg1}, fe_holder{arg2})
        {}
        constexpr add(std::integral auto arg1, std::integral auto arg2)
            : add(static_cast<double>(arg1), static_cast<double>(arg2))
        {}
        constexpr add(Arg1 arg1, std::integral auto arg2)
            : add(arg1, static_cast<double>(arg2))
        {}
        constexpr add(std::integral auto arg1, Arg2 arg2)
            : add(static_cast<double>(arg1), arg2)
        {}

        // ---------------------------------------
        // 処理・検証の実装
        // ---------------------------------------

        static constexpr auto fn(calc_t arg1, calc_t arg2)
        { return arg1 + arg2; }

        static constexpr validate_result_t validate(info_t&& info1, info_t&& info2)
        {
            const auto [e, is_run, return_value] = parent_t::validate_arg_default(info1, info2);
            if (!is_run)
                return {e, is_run, return_value};

            // 無限大-無限大は不正な演算とする
            const auto num_of_infinity = parent_t::count_infinity(info1, info2);
            return (num_of_infinity == 2 && info1.sign() != info2.sign())
                ? validate_result_t{FE_INVALID, false, (calc_t)info_t::make_nan()}
                : validate_result_t{
                    std::fexcept_t{},
                    num_of_infinity == 0,
                    parent_t::get_first_infinity(info1, info2)
                };
        }

        static constexpr std::fexcept_t check_result(info_t&& result, info_t&& arg1, info_t&& arg2)
        {
            // 有限数と有限数の加算から無限が生じているのでオーバーフロー
            if (result.is_infinity())
                return FE_INEXACT | FE_OVERFLOW;

            // 結果がゼロなのに、符号をそろえたオペランドが異なる場合アンダーフロー
            return (result.is_zero() && (calc_t)arg1 != -(calc_t)arg2)
                ? FE_INEXACT | FE_UNDERFLOW
                : parent_t::check_after_default(result, arg1, arg2);
        }
    };
}

#endif
