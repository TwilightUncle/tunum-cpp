#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_MODF_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_MODF_HPP

#include TUNUM_COMMON_INCLUDE(math/trunc.hpp)

namespace tunum::_math_impl
{
    // --------------------------------------
    // オーバーロード
    // --------------------------------------

    template <class T1, std::floating_point T2>
    requires (std::is_arithmetic_v<T1>)
    inline constexpr T2 modf(T1 value, T2* iptr) noexcept
    {
        // 外部で変数が定義されていることから、
        // キャストできないポインタ型を基準に戻り値型を決定
        const auto casted = static_cast<T2>(value);
        if (!std::is_constant_evaluated())
            return std::modf(casted, iptr);
        *iptr = trunc(casted);
        return floating_std_info{casted}.get_decimal_part();
    }

    template <class T1, FeHoldable T2>
    requires (std::is_arithmetic_v<T1> || is_fe_holder_v<T1>)
    inline constexpr T2 modf(const T1& value, T2* iptr) noexcept
    {
        auto casted = T2{value};
        *iptr = trunc(casted);
        casted.value = floating_std_info{casted.value}.get_decimal_part();
        return casted;
    }

    template <class T>
    requires (std::is_arithmetic_v<T> || is_fe_holder_v<T>)
    inline constexpr auto modf_arr(const T& value) noexcept
    {
        using calc_t = integral_to_floating_t<T, double>;
        auto integral_part = calc_t{};
        return std::array<calc_t, 2>{modf(value, &integral_part), integral_part};
    }

    // --------------------------------------
    // cpo
    // --------------------------------------

    struct modf_cpo
    {
        constexpr auto operator()(auto x) const
        { return modf(x); }
    };

    struct modf_arr_cpo
    {
        template <class T>
        constexpr std::array<T, 2> operator()(T value) const
        { return modf_arr(value); }
    };
}

namespace tunum
{

    // 小数部と整数部の分離
    // @param value
    // @param iptr
    inline constexpr _math_impl::modf_cpo modf{};

    // 小数点と整数部の分離
    // modfとの違いは、整数部を第二引数のポインタで受け取るのではなく、
    // 小数部と整数部のペアの値の配列を返却するところ。
    // @param value
    // @return [fractional_part, integral_part]
    inline constexpr _math_impl::modf_arr_cpo modf_arr{};
}

#endif
