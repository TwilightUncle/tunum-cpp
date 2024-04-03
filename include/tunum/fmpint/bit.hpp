// ------------------------------------------
// 標準ライブラリ<bit>と同様の一部機能を、
// fmpintでも同じような感じで使えるように定義
// ------------------------------------------
#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_BIT_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_BIT_HPP

#include TUNUM_COMMON_INCLUDE(fmpint/core.hpp)

namespace tunum
{
    // 一つだけbitが立っているか判定
    template <TuUnsigned T>
    constexpr bool has_single_bit(const T& x) noexcept
    {
        if constexpr (std::unsigned_integral<T>)
            return std::has_single_bit<T>(x);
        else
            return x.count_one_bit() == 1;
    }

    // 値を表現するのに必要なビット幅を取得
    template <TuUnsigned T>
    constexpr int bit_width(const T& x) noexcept
    {
        if constexpr (std::unsigned_integral<T>)
            return std::bit_width<T>(x);
        else
            return x.get_bit_width();
    }

    // 整数値を2の累乗に押し上げる
    template <TuUnsigned T>
    constexpr T bit_ceil(const T& x)
    {
        if (has_single_bit(x))
            return x;
        if constexpr (std::unsigned_integral<T>)
            return std::bit_ceil<T>(x);
        else {
            T v{};
            v.set_bit(tunum::bit_width(x), true);
            return v;
        }
    }

    // 整数を2の累乗に押し下げる
    template <TuUnsigned T>
    constexpr T bit_floor(const T& x)
    {
        if (has_single_bit(x))
            return x;
        if constexpr (std::unsigned_integral<T>)
            return std::bit_floor<T>(x);
        else {
            T v{};
            if (x > 0)
                v.set_bit(tunum::bit_width(x) - 1, true);
            return v;
        }
    }
}

#endif
