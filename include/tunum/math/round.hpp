#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_ROUND_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_ROUND_HPP

#include TUNUM_COMMON_INCLUDE(math/floating.hpp)
#include TUNUM_COMMON_INCLUDE(math/floor.hpp)
#include TUNUM_COMMON_INCLUDE(math/ceil.hpp)

namespace tunum::_math_impl
{
    // --------------------------------------
    // オーバーロード
    // --------------------------------------

    template <class T>
    requires (is_fe_holder_v<T> || std::is_floating_point_v<T>)
    inline constexpr T round(const T& x) noexcept
    {
        if constexpr (std::is_arithmetic_v<T>)
            if (!std::is_constant_evaluated())
                return std::round(x);
        const auto abs_x = copysign(x, T{1});
        const auto abs_result = floating_std_info{abs_x}.get_decimal_part() < T{0.5}
            ? T{floor(abs_x)}
            : T{ceil(abs_x)};
        return copysign(abs_result, x);
    }

    inline constexpr auto round(std::integral auto x) noexcept
    { return round(double(x)); }

    // --------------------------------------
    // cpo
    // --------------------------------------

    struct round_cpo
    {
        constexpr auto operator()(auto x) const
        { return round(x); }
    };
}

namespace tunum
{
    // 四捨五入
    // @param x
    inline constexpr _math_impl::round_cpo round{};
}

#endif
