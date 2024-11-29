#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_NEXTTOWARD_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_NEXTTOWARD_HPP

#include TUNUM_COMMON_INCLUDE(math/nextafter.hpp)

namespace tunum::_math_impl
{
    // --------------------------------------
    // オーバーロード
    // --------------------------------------

    // 組み込みの実装用
    template <class T1, class T2>
    requires (std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>)
    inline constexpr auto nexttoward(T1 x, T2 y)
    {
        // 戻り値の統一のため、整数値はdoubleに変換
        using calc_t1 = integral_to_floating_t<T1, double>;
        using calc_t2 = integral_to_floating_t<T2, double>;
        if (!std::is_constant_evaluated())
            return std::nexttoward(x, y);
        return fe_nextafter{x, y}.value;
    }

    template <class T1, class T2>
    requires (is_fe_holder_v<T1> || is_fe_holder_v<T2>)
    inline constexpr auto nexttoward(T1 x, T2 y)
    { return fe_nextafter{x, y}; }

    // --------------------------------------
    // cpo
    // --------------------------------------

    struct nexttoward_cpo
    {
        constexpr auto operator()(auto x, auto y) const
        { return nexttoward(x, y); }
    };
}

namespace tunum
{
    // xから見た、yの方向へ存在する隣接値を取得
    // x == yの場合は、値が変化しない
    // @param x
    // @param y
    inline constexpr _math_impl::nexttoward_cpo nexttoward{};
}

#endif
