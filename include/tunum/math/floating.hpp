#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_FLOATING_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_FLOATING_HPP

#include <cmath>
#include TUNUM_COMMON_INCLUDE(floating.hpp)

namespace tunum::_math_impl
{
    template <class T>
    requires (std::is_arithmetic_v<T>)
    inline constexpr T copysign(T x, T y) noexcept
    {
        if (!std::is_constant_evaluated())
            return std::copysign(x, y);
        else
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
