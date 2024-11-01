// ------------------------------------------
// 標準ライブラリ<bit>と同様の一部機能を、
// fmpintでも同じような感じで使えるように定義
// ------------------------------------------
#ifndef TUNUM_INCLUDE_GUARD_TUNUM_BIT_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_BIT_HPP

#ifndef TUNUM_COMMON_INCLUDE
#define TUNUM_COMMON_INCLUDE(path) <tunum/path>
#endif

#include TUNUM_COMMON_INCLUDE(concepts.hpp)

namespace tunum::_bit_impl
{
    using std::rotl;
    template <TuFmpUnsigned T>
    constexpr T rotl(const T& x, int s) noexcept
    { return x.rotate_l(s); }

    using std::rotr;
    template <TuFmpUnsigned T>
    constexpr T rotr(const T& x, int s) noexcept
    { return x.rotate_r(s); }

    using std::countl_zero;
    constexpr int countl_zero(const TuFmpUnsigned auto& x) noexcept
    { return x.countl_zero_bit(); }

    using std::countr_zero;
    constexpr int countr_zero(const TuFmpUnsigned auto& x) noexcept
    { return x.countr_zero_bit(); }

    using std::countl_one;
    constexpr int countl_one(const TuFmpUnsigned auto& x) noexcept
    { return x.countl_one_bit(); }

    using std::countr_one;
    constexpr int countr_one(const TuFmpUnsigned auto& x) noexcept
    { return x.countr_one_bit(); }

    using std::popcount;
    constexpr int popcount(const TuFmpUnsigned auto& x) noexcept
    { return x.count_one_bit(); }

    using std::has_single_bit;
    constexpr bool has_single_bit(const TuFmpUnsigned auto& x) noexcept
    { return x.count_one_bit() == 1; }

    using std::bit_width;
    constexpr int bit_width(const TuFmpUnsigned auto& x) noexcept
    { return x.get_bit_width(); }

    using std::bit_ceil;
    template <TuFmpUnsigned T>
    constexpr T bit_ceil(const T& x) noexcept
    {
        if (has_single_bit(x))
            return x;
        T v{};
        v.set_bit(bit_width(x), true);
        return v;
    }

    using std::bit_floor;
    template <TuFmpUnsigned T>
    constexpr T bit_floor(const T& x) noexcept
    {
        if (has_single_bit(x))
            return x;
        T v{};
        if (x > 0)
            v.set_bit(bit_width(x) - 1, true);
        return v;
    }

    struct rotl_cpo
    {
        template <TuBitwiseOperable T>
        [[nodiscard]] constexpr T operator()(const T& x, int s) const noexcept
        { return rotl(x, s); } 
    };

    struct rotr_cpo
    {
        template <TuBitwiseOperable T>
        [[nodiscard]] constexpr T operator()(const T& x, int s) const noexcept
        { return rotr(x, s); } 
    };

    struct countl_zero_cpo
    {
        constexpr int operator()(const TuBitwiseOperable auto& x) const noexcept
        { return countl_zero(x); } 
    };

    struct countr_zero_cpo
    {
        constexpr int operator()(const TuBitwiseOperable auto& x) const noexcept
        { return countr_zero(x); } 
    };

    struct countl_one_cpo
    {
        constexpr int operator()(const TuBitwiseOperable auto& x) const noexcept
        { return countl_one(x); } 
    };

    struct countr_one_cpo
    {
        constexpr int operator()(const TuBitwiseOperable auto& x) const noexcept
        { return countr_one(x); } 
    };

    struct popcount_cpo
    {
        constexpr int operator()(const TuBitwiseOperable auto& x) const noexcept
        { return popcount(x); } 
    };

    struct has_single_bit_cpo
    {
        constexpr bool operator()(const TuBitwiseOperable auto& x) const noexcept
        { return has_single_bit(x); } 
    };

    struct bit_width_cpo
    {
        constexpr int operator()(const TuBitwiseOperable auto& x) const noexcept
        { return bit_width(x); } 
    };

    struct bit_ceil_cpo
    {
        template <TuBitwiseOperable T>
        constexpr T operator()(const T& x) const noexcept
        { return bit_ceil(x); } 
    };

    struct bit_floor_cpo
    {
        template <TuBitwiseOperable T>
        constexpr T operator()(const T& x) const noexcept
        { return bit_floor(x); } 
    };
}

namespace tunum
{
    // 左循環シフト
    inline constexpr _bit_impl::rotl_cpo rotl{};

    // 右循環シフト
    inline constexpr _bit_impl::rotr_cpo rotr{};

    // 左から連続した0のビットを数える
    inline constexpr _bit_impl::countl_zero_cpo countl_zero{};

    // 右から連続し0のビットを数える
    inline constexpr _bit_impl::countr_zero_cpo countr_zero{};

    // 左から連続した1のビットを数える
    inline constexpr _bit_impl::countl_one_cpo countl_one{};

    // 右から連続した1のビットを数える
    inline constexpr _bit_impl::countr_one_cpo countr_one{};

    // 立っているビット数をカウント
    inline constexpr _bit_impl::popcount_cpo popcount{};

    // 一つだけbitが立っているか判定
    inline constexpr _bit_impl::has_single_bit_cpo has_single_bit{};

    // 値を表現するのに必要なビット幅を取得
    inline constexpr _bit_impl::bit_width_cpo bit_width{};

    // 整数値を2の累乗に押し上げる
    inline constexpr _bit_impl::bit_ceil_cpo bit_ceil{};

    // 整数を2の累乗に押し下げる
    inline constexpr _bit_impl::bit_floor_cpo bit_floor{};
}

#endif
