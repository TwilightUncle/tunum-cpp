#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_FLOATING_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_FLOATING_HPP

#include <cmath>
#include TUNUM_COMMON_INCLUDE(floating.hpp)

namespace tunum::_math_impl
{
    template <std::floating_point T>
    inline constexpr int fpclassify(T x) noexcept
    {
        constexpr bool is_callable_std_defined = requires {
            typename require_constant<std::fpclassify(T{})>;
        };
        if constexpr (is_callable_std_defined)
            return std::fpclassify(x);
        if (!std::is_constant_evaluated())
            return std::fpclassify(x);
        return floating_std_info{x}.get_fpclass();
    }

    inline constexpr int fpclassify(std::integral auto x) noexcept
    { return fpclassify(double(x)); }

    template <std::floating_point T>
    inline constexpr bool isfinite(T x) noexcept
    {
        constexpr bool is_callable_std_defined = requires {
            typename require_constant<std::isfinite(T{})>;
        };
        if constexpr (is_callable_std_defined)
            return std::isfinite(x);
        if (!std::is_constant_evaluated())
            return std::isfinite(x);
        return floating_std_info{x}.is_finity();
    }

    inline constexpr bool isfinite(std::integral auto x) noexcept
    { return isfinite(double(x)); }

    template <std::floating_point T>
    inline constexpr bool isinf(T x) noexcept
    {
        constexpr bool is_callable_std_defined = requires {
            typename require_constant<std::isinf(T{})>;
        };
        if constexpr (is_callable_std_defined)
            return std::isinf(x);
        if (!std::is_constant_evaluated())
            return std::isinf(x);
        return floating_std_info{x}.is_infinity();
    }

    inline constexpr bool isinf(std::integral auto x) noexcept
    { return isinf(double(x)); }

    template <std::floating_point T>
    inline constexpr bool isnan(T x) noexcept
    {
        constexpr bool is_callable_std_defined = requires {
            typename require_constant<std::isnan(T{})>;
        };
        if constexpr (is_callable_std_defined)
            return std::isnan(x);
        if (!std::is_constant_evaluated())
            return std::isnan(x);
        return floating_std_info{x}.is_nan();
    }
    
    inline constexpr bool isnan(std::integral auto x) noexcept
    { return isnan(double(x)); }

    template <std::floating_point T>
    inline constexpr bool isnormal(T x) noexcept
    {
        constexpr bool is_callable_std_defined = requires {
            typename require_constant<std::isnormal(T{})>;
        };
        if constexpr (is_callable_std_defined)
            return std::isnormal(x);
        if (!std::is_constant_evaluated())
            return std::isnormal(x);
        return floating_std_info{x}.is_normalized();
    }

    inline constexpr bool isnormal(std::integral auto x) noexcept
    { return isnormal(double(x)); }

    template <std::floating_point T>
    inline constexpr bool signbit(T x) noexcept
    {
        constexpr bool is_callable_std_defined = requires {
            typename require_constant<std::signbit(T{})>;
        };
        if constexpr (is_callable_std_defined)
            return std::signbit(x);
        if (!std::is_constant_evaluated())
            return std::signbit(x);
        return floating_std_info{x}.sign() < 0;
    }

    inline constexpr bool signbit(std::integral auto x) noexcept
    { return signbit(double(x)); }
    
    template <class T>
    requires (std::is_arithmetic_v<T>)
    inline constexpr T copysign(T x, T y) noexcept
    {
        constexpr bool is_callable_std_defined = requires {
            typename require_constant<std::copysign(T{}, T{})>;
        };
        if constexpr (is_callable_std_defined)
            return std::copysign(x, y);
        if (!std::is_constant_evaluated())
            return std::copysign(x, y);
        if constexpr (std::floating_point<T>)
            return (T)floating_std_info{x}.change_sign(signbit(y));
        return (x < 0) == (y < 0) 
            ? x
            : x * -1;
    }

    template <std::floating_point T>
    inline constexpr T trunc(T x) noexcept
    {
        if (!std::is_constant_evaluated())
            return std::trunc(x);
        return floating_std_info{x}.get_integral_part();
    }

    inline constexpr double trunc(std::integral auto x) noexcept
    { return trunc(double(x)); }

    template <std::floating_point T>
    inline constexpr T round(T x) noexcept
    {
        if (!std::is_constant_evaluated())
            return std::round(x);
        const auto abs_x = copysign(x, T{1});
        const auto abs_result = floating_std_info{abs_x}.get_decimal_part() < T{0.5}
            ? floor(abs_x)
            : ceil(abs_x);
        return copysign(abs_result, x);
    }

    inline constexpr double round(std::integral auto x) noexcept
    { return round(double(x)); }

    template <std::floating_point T>
    inline constexpr T modf(T value, T* iptr) noexcept
    {
        if (!std::is_constant_evaluated())
            return std::modf(value, iptr);
        *iptr = trunc(value);
        return floating_std_info{value}.get_decimal_part();
    }

    template <std::floating_point T>
    inline constexpr std::array<T, 2> modf_arr(T value) noexcept
    {
        auto integral_part = T{};
        return {modf(value, &integral_part), integral_part};
    }

    // -------------------------------------------
    // cpo
    // -------------------------------------------

    struct fpclassify_cpo
    {
        constexpr int operator()(auto x) const
        { return fpclassify(x); }
    };

    struct isfinite_cpo
    {
        constexpr bool operator()(auto x) const
        { return isfinite(x); }
    };

    struct isinf_cpo
    {
        constexpr bool operator()(auto x) const
        { return isinf(x); }
    };

    struct isnan_cpo
    {
        constexpr bool operator()(auto x) const
        { return isnan(x); }
    };

    struct isnormal_cpo
    {
        constexpr bool operator()(auto x) const
        { return isnormal(x); }
    };

    struct signbit_cpo
    {
        constexpr bool operator()(auto x) const
        { return signbit(x); }
    };

    struct copysign_cpo
    {
        template <class T>
        constexpr T operator()(T x, T y) const
        { return copysign(x, y); }
    };

    struct trunc_cpo
    {
        constexpr auto operator()(auto x) const
        { return trunc(x); }
    };

    struct round_cpo
    {
        constexpr auto operator()(auto x) const
        { return round(x); }
    };

    struct modf_cpo
    {
        template <class T>
        constexpr T operator()(T value, T* iptr) const
        { return modf(value, iptr); }
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
    // 数値分類を取得
    // @param x 検査対象
    // @return FP_INFINITE, FP_NAN, FP_NORMAL, FP_SUBNORMAL, FP_ZEROのいずれか、あるいは処理系独自定義の数値分類
    inline constexpr _math_impl::fpclassify_cpo fpclassify{};

    // 有限か判定
    // @param x 検査対象
    inline constexpr _math_impl::isfinite_cpo isfinite{};

    // 無限大か判定
    // @param x 検査対象
    inline constexpr _math_impl::isinf_cpo isinf{};

    // NaNであるか判定
    // @param x 検査対象
    inline constexpr _math_impl::isnan_cpo isnan{};

    // 正規化数であるか判定
    // @param x 検査対象
    inline constexpr _math_impl::isnormal_cpo isnormal{};

    // 負数であるか判定
    // @param x 検査対象
    inline constexpr _math_impl::signbit_cpo signbit{};

    // 符号のコピー
    // @param x 値担当
    // @param y 符号担当
    inline constexpr _math_impl::copysign_cpo copysign{};

    // 0方向への丸め
    // @param x
    inline constexpr _math_impl::trunc_cpo trunc{};

    // 四捨五入
    // @param x
    inline constexpr _math_impl::round_cpo round{};

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
