#ifndef TUNUM_INCLUDE_GUARD_TUNUM_UTILITY_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_UTILITY_HPP

#ifndef TUNUM_COMMON_INCLUDE
#define TUNUM_COMMON_INCLUDE(path) <tunum/path>
#endif

#include <concepts>
#include <numbers>
#include <bit>

namespace tunum
{
    // 算術型か、整数値と下記の操作ができる型
    // - 全順序比較
    // - 四則演算
    // - 単項 マイナス
    template <class T>
    concept TuArithmetic
        = requires (T v) {
            { v } -> std::totally_ordered_with<int>;
            { v.operator-() } -> std::convertible_to<T>;
            { v + std::declval<int>() } -> std::convertible_to<T>;
            { v - std::declval<int>() } -> std::convertible_to<T>;
            { v * std::declval<int>() } -> std::convertible_to<T>;
            { v / std::declval<int>() } -> std::convertible_to<T>;
        }
        || std::is_arithmetic_v<T>;

    // 絶対値取得
    inline constexpr auto abs(const TuArithmetic auto& v) { return v < 0 ? -v : v; }

    // 小数点以下切り捨て
    inline constexpr auto floor(std::floating_point auto v) { return static_cast<std::int64_t>(v); }

    // 小数点以下切り上げ
    inline constexpr auto ceil(std::floating_point auto v)
    {
        const auto floored = tunum::floor(v);
        return floored < v
            ? floored + 1
            : floored;
    }

    // ビット列を指定サイズを基準とした領域へアライメント
    // 格納するのに要する、基準サイズの要素数を返却
    // デジタル版ceil
    inline constexpr auto alignment(std::size_t bit_width, std::size_t base_size = 8)
    {
        return bit_width / base_size + bool(bit_width % base_size);
    }

    // 指定桁数の整数を確実に格納可能なバイト数を計算。
    // @tparam Base 進数を指定。2, 8, 16 以外は全て 10 進数として処理
    // @param digits 桁数
    template <std::size_t Base = 10>
    inline constexpr auto calc_integral_storable_byte(std::size_t digits)
    {
        constexpr auto byte = 8u;
        if constexpr (Base == 2 || Base == 8 || Base == 16) {
            constexpr auto base_bit_width = std::bit_width(Base) - 1;
            const auto total_bit_width = digits * base_bit_width;
            return tunum::alignment(total_bit_width, byte);
        }
        else {
            constexpr auto log10_2 = std::numbers::ln2 / std::numbers::ln10;
            // 2, 8, 16 進数以外は全て 10 進数として処理する
            const auto near_total_bit_width = digits / log10_2;
            return tunum::alignment(tunum::ceil(near_total_bit_width), byte);
        }
    }

} // namespace tunum

#endif
