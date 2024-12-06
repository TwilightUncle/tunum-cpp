#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_FLOATING_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_FLOATING_HPP

#include <cmath>
#include TUNUM_COMMON_INCLUDE(floating.hpp)

namespace tunum::_math_impl
{
    // fpclassifyのオーバーロード--------------------------------------

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

    template <class T>
    requires (is_fe_holder_v<T>)
    inline constexpr int fpclassify(const T& x) noexcept
    { return fpclassify(x.value); }

    // isfiniteのオーバーロード----------------------------------------

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

    template <class T>
    requires (is_fe_holder_v<T>)
    inline constexpr bool isfinite(const T& x) noexcept
    { return isfinite(x.value); }

    // isinfのオーバーロード----------------------------------------

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

    template <class T>
    requires (is_fe_holder_v<T>)
    inline constexpr bool isinf(const T& x) noexcept
    { return isinf(x.value); }

    // isnanのオーバーロード-----------------------------------------

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

    template <class T>
    requires (is_fe_holder_v<T>)
    inline constexpr bool isnan(const T& x) noexcept
    { return isnan(x.value); }

    // isnormalのオーバーロード-----------------------------------------

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

    template <class T>
    requires (is_fe_holder_v<T>)
    inline constexpr bool isnormal(const T& x) noexcept
    { return isnormal(x.value); }

    // signbitのオーバーロード-----------------------------------------

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
    requires (is_fe_holder_v<T>)
    inline constexpr bool signbit(const T& x) noexcept
    { return signbit(x.value); }

    // copysignのオーバーロード-----------------------------------------

    template <class T1, class T2>
    requires (
        tump::count_if_v<tump::is_arithmetic, tump::list<T1, T2>>
        + tump::count_if_v<::tunum::is_fe_holder, tump::list<T1, T2>>
        == 2
    )
    inline constexpr T1 copysign(T1 x, T2 y) noexcept
    {
        using calc_t2 = integral_to_floating_t<T2, double>;
        if constexpr (std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>)
            if (!std::is_constant_evaluated())
                return std::copysign(x, y);
        if constexpr (is_fe_holder_v<T1>) {
            x.value = copysign(x.value, calc_t2(y));
            return fe_holder{x};
        }
        if constexpr (std::floating_point<T1>)
            return (T1)floating_std_info{x}.change_sign(signbit(y));
        return copysign(double(x), calc_t2(y));
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
        constexpr auto operator()(auto x, auto y) const
        { return copysign(x, y); }
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
}

#endif
