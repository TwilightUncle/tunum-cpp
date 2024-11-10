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

        constexpr add(const fe_holder<Arg1>& arg1, const fe_holder<Arg2>& arg2)
            : parent_t(fn, validate, arg1, arg2)
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

        static constexpr auto fn = [](calc_t arg1, calc_t arg2) {
            return arg1 + arg2;
        };

        static constexpr auto validate = [](info_t&& info1, info_t&& info2) -> validate_result_t {
            if (info1.is_nan() || info2.is_nan())
                return {std::fexcept_t{}, false, (calc_t)info_t::make_nan()};
            // 無限大-無限大は不正な演算とする
            if (info1.is_infinity() && info2.is_infinity() && info1.sign() != info2.sign())
                return validate_result_t{FE_INVALID, false, (calc_t)info_t::make_nan()};
            if (!info1.is_infinity() && !info2.is_infinity())
                return {std::fexcept_t{}, true, calc_t{}};
            return {
                std::fexcept_t{},
                false,
                (calc_t)(info1.is_infinity() ? info1 : info2)
            };
        };

        static constexpr auto check_result = [](info_t&& result, info_t&& arg1, info_t&& arg2) -> std::fexcept_t {
            if (result.is_infinity())
                return FE_INEXACT | FE_OVERFLOW;
            // 結果がゼロなのに、符号をそろえたオペランドが異なる場合アンダーフロー
            if (result.is_zero() && (calc_t)arg1 != -(calc_t)arg2)
                return FE_INEXACT | FE_UNDERFLOW;
            // 非正規化数周り
            return std::fexcept_t{};
        };
    };
}

#endif
