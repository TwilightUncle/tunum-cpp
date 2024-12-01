#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_FLOOR_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_FLOOR_HPP

#include <cmath>
#include TUNUM_COMMON_INCLUDE(floating.hpp)

namespace tunum::_math_impl
{
    // --------------------------------------
    // fe_holder向けの実装
    // --------------------------------------

    template <std::floating_point T, std::fexcept_t RaiseFeFlags>
    struct fe_floor : public fe_fn<RaiseFeFlags, T>
    {
        using parent_t = fe_fn<RaiseFeFlags, T>;
        using calc_t = typename parent_t::calc_t;
        using info_t = typename parent_t::info_t;
        using validate_result_t = typename parent_t::validate_result_t;

        // -------------------------------------
        // コンストラクタ
        // -------------------------------------

        constexpr fe_floor(
            const fe_holder<T, RaiseFeFlags>& arg
        )
            : parent_t(fn, validate, check_result, arg)
        {}

        // ---------------------------------------
        // 処理・検証の実装
        // ---------------------------------------

        static constexpr auto fn = [](calc_t arg)
        {
            const auto info = info_t{arg};
            return (info.has_decimal_part() && arg < 0)
                ? info.get_integral_part() - 1
                : info.get_integral_part();
        };

        static constexpr auto validate = [](const info_t& info) -> validate_result_t
        {
            const auto [e, is_run, return_value] = parent_t::validate_arg_default(info);
            return info.is_infinity()
                ? validate_result_t{e, false, calc_t(info)}
                : validate_result_t{e, is_run, return_value};
        };

        static constexpr auto check_result = [](const info_t&, const info_t&) -> std::fexcept_t
        { return std::fexcept_t{}; };
    };

    // --------------------------------------
    // 推論補助
    // --------------------------------------

    template <class T>
    requires (std::is_arithmetic_v<T>)
    fe_floor(T)
        -> fe_floor<integral_to_floating_t<T, double>, std::fexcept_t{}>;

    // --------------------------------------
    // オーバーロード
    // --------------------------------------

    // 算術型に対するオーバーロード
    template <class T>
    requires (std::is_arithmetic_v<T>)
    inline constexpr T floor(T x) noexcept
    {
        if (!std::is_constant_evaluated())
            return std::floor(x);
        return fe_floor(x).value;
    }

    // fe_holderに対するオーバーロード
    template <class T>
    requires (is_fe_holder_v<T>)
    inline constexpr auto floor(const T& x) noexcept
    { return fe_floor(double(x)); }

    // --------------------------------------
    // cpo
    // --------------------------------------

    struct floor_cpo
    {
        constexpr auto operator()(auto x) const
        { return floor(x); }
    };
}

namespace tunum
{
    // 床関数
    // @param x
    inline constexpr _math_impl::floor_cpo floor{};
}

#endif