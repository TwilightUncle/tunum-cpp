#ifndef TUNUM_INCLUDE_GUARD_TUNUM_UTILITY_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_UTILITY_HPP

#ifndef TUNUM_COMMON_INCLUDE
#define TUNUM_COMMON_INCLUDE(path) <tunum/path>
#endif

#include <concepts>
#include <numbers>
#include <bit>
#include <string_view>

#ifndef TUNUM_MAKE_ANY_TYPE_STR_VIEW
// 任意の文字型のstring_viewを生成
// @param char_type string_viewの文字型
// @param traits string_viewのtraits
// @param str_literal char型の文字列リテラルを指定すること
#define TUNUM_MAKE_ANY_TYPE_STR_VIEW(char_type, traits, str_literal) []() -> std::basic_string_view<char_type, traits> { \
        if constexpr (std::is_same_v<char_type, wchar_t>) return {(L ## str_literal)}; \
        else if constexpr (std::is_same_v<char_type, char8_t>) return {(u8 ## str_literal)}; \
        else if constexpr (std::is_same_v<char_type, char16_t>) return {(u ## str_literal)}; \
        else if constexpr (std::is_same_v<char_type, char32_t>) return {(U ## str_literal)}; \
        else return {(str_literal)}; \
    }()
#endif

namespace tunum
{
    // 算術型か、整数値と下記の操作ができる型
    // - 全順序比較
    // - 四則演算
    // - 単項 マイナス
    template <class T>
    concept TuArithmetic
        = requires (T v) {
            { v } -> std::totally_ordered;
            { v } -> std::totally_ordered_with<int>;
            { v.operator-() } -> std::convertible_to<T>;
            { v + std::declval<int>() } -> std::convertible_to<T>;
            { v + std::declval<T>() } -> std::convertible_to<T>;
            { v - std::declval<int>() } -> std::convertible_to<T>;
            { v - std::declval<T>() } -> std::convertible_to<T>;
            { v * std::declval<int>() } -> std::convertible_to<T>;
            { v * std::declval<T>() } -> std::convertible_to<T>;
            { v / std::declval<int>() } -> std::convertible_to<T>;
            { v / std::declval<T>() } -> std::convertible_to<T>;
        }
        || std::is_arithmetic_v<T>;

    // 絶対値取得
    inline constexpr auto abs(const TuArithmetic auto& v) { return (std::max)(v, -v); }

    // 自然対数の近似値算出
    // @tparam FloatT 任意の組み込み浮動小数点型
    // @tparam N 求める項の数
    // @param x 求めたい自然対数の真数
    template <std::floating_point FloatT, std::size_t N = 60>
    requires (N > 1)
    inline constexpr FloatT ln(FloatT x)
    {
        if (x <= 0)
            throw std::invalid_argument("Values less than 0 cannot be specified.");

        const FloatT base_numerator = x - 1;
        if (abs(base_numerator) >= 1)
            return -ln<FloatT, N>(1 / x);
        if (x < .5)
            return ln<FloatT, N>(.5) + ln<FloatT, N>(2 * x);

        FloatT numerator = base_numerator;
        FloatT total = 0;
        for (int i = 1, sign = 1; i < N; i++) {
            total += (sign * numerator / i);
            sign *= -1;
            numerator *= base_numerator;
        }
        return total;
    }

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

    namespace numbers
    {
        // ---------------------------
        // 数値の定数定義
        // ---------------------------

        inline constexpr double log10_2 = std::numbers::ln2 / std::numbers::ln10;
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
            constexpr auto base_bit_width = std::bit_width(Base - 1);
            const auto total_bit_width = digits * base_bit_width;
            return tunum::alignment(total_bit_width, byte);
        }
        else {
            // 2, 8, 16 進数以外は全て 10 進数として処理する
            const auto near_total_bit_width = digits / tunum::numbers::log10_2;
            return tunum::alignment(tunum::ceil(near_total_bit_width), byte);
        }
    }

    // 指定のビット幅に対して、Baseの進数を何桁格納できるか計算
    // @tparam Base 進数を指定。2, 8, 16 以外は全て 10 進数として処理
    // @param bit_width 計算したい領域のビット幅
    template <std::size_t Base = 10>
    inline constexpr auto calc_integral_digits_from_bitwidth(std::size_t bit_width)
    {
        if constexpr (Base == 2 || Base == 8 || Base == 16) 
            return alignment(bit_width, std::bit_width(Base) - 1);
        else
            return static_cast<std::size_t>(bit_width * tunum::numbers::log10_2);
    }
} // namespace tunum

#endif
