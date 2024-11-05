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

    // ceil内での呼び出しが認識されるように、宣言
    template <std::floating_point T>
    inline constexpr T floor(T x) noexcept;

    template <std::floating_point T>
    inline constexpr T ceil(T x) noexcept
    {
        if (!std::is_constant_evaluated())
            return std::ceil(x);
        const auto info = floating_std_info{x};
        return x < 0
            ? -floor(-x)
            : info.get_integral_part() + T(info.has_decimal_part());
    }

    inline constexpr double ceil(std::integral auto x) noexcept
    { return ceil(double(x)); }

    template <std::floating_point T>
    inline constexpr T floor(T x) noexcept
    {
        if (!std::is_constant_evaluated())
            return std::floor(x);
        return x < 0
            ? -ceil(-x)
            : floating_std_info{x}.get_integral_part();
    }

    inline constexpr double floor(std::integral auto x) noexcept
    { return floor(double(x)); }

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

    struct ceil_cpo
    {
        constexpr auto operator()(auto x) const
        { return ceil(x); }
    };

    struct floor_cpo
    {
        constexpr auto operator()(auto x) const
        { return floor(x); }
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

    // 天井関数
    // @param x
    inline constexpr _math_impl::ceil_cpo ceil{};

    // 床関数
    // @param x
    inline constexpr _math_impl::floor_cpo floor{};

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
