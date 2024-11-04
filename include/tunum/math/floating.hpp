#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_FLOATING_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_FLOATING_HPP

#include <cmath>
#include TUNUM_COMMON_INCLUDE(floating.hpp)

namespace tunum::_math_impl
{
    template <std::floating_point T>
    inline constexpr T fma(T x, T y, T z) noexcept
    {
        if (!std::is_constant_evaluated())
            return std::fma(x, y, z);
        return x * y + z;
    }

    inline constexpr int fpclassify(std::floating_point auto x) noexcept
    {
        if (!std::is_constant_evaluated())
            return std::fpclassify(x);
        return floating_std_info{x}.get_fpclass();
    }

    inline constexpr int fpclassify(std::integral auto x) noexcept
    { return fpclassify(double(x)); }

    inline constexpr bool isfinite(std::floating_point auto x) noexcept
    {
        if (!std::is_constant_evaluated())
            return std::isfinite(x);
        return floating_std_info{x}.is_finity();
    }

    inline constexpr bool isfinite(std::integral auto x) noexcept
    { return isfinite(double(x)); }

    inline constexpr bool isinf(std::floating_point auto x) noexcept
    {
        if (!std::is_constant_evaluated())
            return std::isinf(x);
        return floating_std_info{x}.is_infinity();
    }

    inline constexpr bool isinf(std::integral auto x) noexcept
    { return isinf(double(x)); }

    inline constexpr bool isnan(std::floating_point auto x) noexcept
    {
        if (!std::is_constant_evaluated())
            return std::isnan(x);
        return floating_std_info{x}.is_nan();
    }
    
    inline constexpr bool isnan(std::integral auto x) noexcept
    { return isnan(double(x)); }

    inline constexpr bool isnormal(std::floating_point auto x) noexcept
    {
        if (!std::is_constant_evaluated())
            return std::isnan(x);
        return floating_std_info{x}.is_normalized();
    }

    inline constexpr bool isnormal(std::integral auto x) noexcept
    { return isnormal(double(x)); }

    inline constexpr bool signbit(std::floating_point auto x) noexcept
    {
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
        if (!std::is_constant_evaluated())
            return std::copysign(x, y);
        return (x < 0) == (y < 0) 
            ? x
            : x * -1;
    }

    template <std::floating_point T>
    inline constexpr T nextafter(T x, T y)
    {
        if (!std::is_constant_evaluated())
            return std::nextafter(x, y);
        return floating_std_info{x}.nextafter(y);
    }

    template <std::integral T>
    inline constexpr double nextafter(T x, T y)
    { return nextafter(double(x), double(y)); }

    template <class T>
    requires (std::is_arithmetic_v<T>)
    inline constexpr auto nexttoward(T x, T y)
    {
        if (!std::is_constant_evaluated())
            return std::nexttoward(x, y);
        return nextafter(x, y);
    }

    // -------------------------------------------
    // cpo
    // -------------------------------------------

    struct fma_cpo
    {
        template <class T>
        constexpr T operator()(T x, T y, T z) const
        { return fma(x, y, z); }
    };

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

    struct nextafter_cpo
    {
        template <class T>
        constexpr T operator()(T x, T y) const
        { return nextafter(x, y); }
    };

    struct nexttoward_cpo
    {
        template <class T>
        constexpr T operator()(T x, T y) const
        { return nexttoward(x, y); }
    };
}

namespace tunum
{
    // 乗算と加算の合成
    // 実行時かつfma命令が有効であれば、fma命令を実行
    // 上記以外では x * y + z
    // @param x
    // @param y
    // @param z
    // @return x * y + z
    inline constexpr _math_impl::fma_cpo fma{};

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

    // 隣接する表現の値
    // @param from 元の値
    // @param to 取得する値の方向
    inline constexpr _math_impl::nextafter_cpo nextafter{};

    // 隣接する表現の値
    // @param from 元の値
    // @param to 取得する値の方向
    inline constexpr _math_impl::nexttoward_cpo nexttoward{};
}

#endif
