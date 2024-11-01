#ifndef TUNUM_INCLUDE_GUARD_TUNUM_UTILITY_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_UTILITY_HPP

#ifndef TUNUM_COMMON_INCLUDE
#define TUNUM_COMMON_INCLUDE(path) <tunum/path>
#endif

#include <string_view>

#include TUNUM_COMMON_INCLUDE(math.hpp)

#ifndef TUNUM_MAKE_ANY_TYPE_STR_VIEW
// 任意の文字型のstring_viewを生成
// @param char_type string_viewの文字型
// @param traits string_viewのtraits
// @param str_literal char型の文字列リテラルを指定すること
#define TUNUM_MAKE_ANY_TYPE_STR_VIEW(char_type, traits, str_literal) []() noexcept -> std::basic_string_view<char_type, traits> { \
        if constexpr (std::is_same_v<char_type, wchar_t>) return {(L ## str_literal)}; \
        else if constexpr (std::is_same_v<char_type, char8_t>) return {(u8 ## str_literal)}; \
        else if constexpr (std::is_same_v<char_type, char16_t>) return {(u ## str_literal)}; \
        else if constexpr (std::is_same_v<char_type, char32_t>) return {(U ## str_literal)}; \
        else return {(str_literal)}; \
    }()
#endif

namespace tunum
{
    // 数値配列の内容を検証  
    // @param base 入力した数値列の進数
    // @param numbers 検査対象の数値列
    inline constexpr bool validate_numbers(std::size_t base, const TuAnyBaseNumbers auto& numbers) noexcept
    {
        // 格納されている値が該当進数の一桁に収まっているか検証
        for (auto num : numbers)
            if (num >= base || 0 > num)
                return false;
        return true;
    }

    // 進数変換結果の桁数を計算
    // TODO: 多倍長浮動小数点型によりぴったりの桁数を出せるか考慮。基本的に割り算が重いため、コンパイル時には使い物にならない気もするが
    // @param from_base 変換前の進数
    // @param to_base 変換後の進数
    // @param from_numbers 変換前の数値列
    inline constexpr std::size_t calc_base_number_digits(
        std::size_t from_base,
        std::size_t to_base,
        const TuAnyBaseNumbers auto& numbers
    ) {
        if (!validate_numbers(from_base, numbers))
            throw std::invalid_argument("Specified invalid numbers.");

        const std::size_t len = std::size(numbers);
        if (from_base == to_base)
            return len;

        double max_v{}, cur_v{}, r = 1.;
        for (std::size_t i = 0; i < len; i++) {
            max_v += double(from_base - 1) * r;
            cur_v += double(numbers[i]) * r;
            r /= from_base;
        }

        // 数値列の値が0の場合は自明
        if (cur_v == 0)
            return 1;

        const double value_rate = ::tunum::log(max_v, cur_v);
        const double digits_rate = ::tunum::log(to_base, from_base);
        return ::tunum::ceil((value_rate + len - 1) * digits_rate);
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
