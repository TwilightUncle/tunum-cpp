#ifndef TUNUM_INCLUDE_GUARD_TUNUM_NUMBER_ARRAY_CONVERT_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_NUMBER_ARRAY_CONVERT_HPP

#include <array>
#include <tuple>

#include TUNUM_COMMON_INCLUDE(utility.hpp)

namespace tunum
{
    // ----------------------------------------------------
    // 文字列 -> 数値列の変換関連処理
    // ----------------------------------------------------

    // 文字を数値へ変換
    // @tparam CharT 文字型
    // @param v 数値に変換したい文字
    template <class CharT>
    inline constexpr auto convert_char_to_num(CharT v) noexcept
    {
        using traits_t = std::char_traits<CharT>;
        constexpr CharT code_zero = TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, traits_t, "0")[0];
        constexpr CharT code_a = TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, traits_t, "a")[0];
        constexpr CharT code_A = TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, traits_t, "A")[0];

        if (v >= code_zero && v < code_zero + 10)
            return int(v - code_zero);
        if (v >= code_a && v < code_a + 6)
            return int(v - code_a + 10);
        return int(v - code_A + 10);
    }

    // 進数リテラルの接頭詞を返却
    // @tparam CharT 文字型
    // @tparam Traits 文字特性
    // @param base_number 進数
    template <class CharT, class Traits = std::char_traits<CharT>>
    inline constexpr auto get_literal_prefixes(std::size_t base_number) noexcept
        -> std::array<std::basic_string_view<CharT, Traits>, 2>
    {
        switch (base_number) {
            case 2:
                return {
                    TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, Traits, "0b"),
                    TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, Traits, "0B")
                };
            case 8:
                return {
                    TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, Traits, "0")
                };
            case 16:
                return {
                    TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, Traits, "0x"),
                    TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, Traits, "0X")
                };
        }
        return {};
    }

    // 接頭詞の長さ取得
    // @param base_number 進数
    inline constexpr std::size_t get_literal_prefix_length(std::size_t base_number) noexcept
    { return (base_number == 2 || base_number == 16) ? 2 : 0; }

    // リテラルの接頭詞から進数を決定
    // @tparam CharT 文字型
    // @tparam Traits 文字特性
    // @param num_str 対象の数値文字列
    template <class CharT, class Traits = std::char_traits<CharT>>
    inline constexpr std::size_t get_base_number(std::basic_string_view<CharT, Traits> num_str)
    {
        constexpr auto all_prefixes = std::array{
            std::pair{2, get_literal_prefixes<CharT, Traits>(2)},
            std::pair{16, get_literal_prefixes<CharT, Traits>(16)},
            std::pair{8, get_literal_prefixes<CharT, Traits>(8)}
        };

        for (const auto [base_number, prefixes] : all_prefixes)
            for (auto prefix : prefixes)
                if (!prefix.empty() && num_str.starts_with(prefix))
                    return base_number;
        return 10;
    }
    
    // リテラルの接頭詞から進数を決定
    // @tparam CharT 文字型
    // @param num_str 対象の数値文字列
    template <class CharT>
    inline constexpr std::size_t get_base_number(const CharT* num_str)
    { return get_base_number(std::basic_string_view<CharT>{num_str}); }

    // リテラルの接頭詞や、先頭の0、区切り文字のシングルクオテーションマーク以外をカウント
    // @tparam CharT 文字型
    // @tparam Traits 文字特性
    // @param num_str 対象の数値文字列
    template <class CharT, class Traits = std::char_traits<CharT>>
    inline constexpr std::size_t count_number_digits(std::basic_string_view<CharT, Traits> num_str)
    {
        // 文字列長から減産していく
        std::size_t cnt = num_str.length();

        // 接頭詞除去
        const auto base_number = get_base_number(num_str);
        const auto prefix_len = get_literal_prefix_length(base_number);
        cnt -= prefix_len;
        num_str.remove_prefix(prefix_len);

        // 先頭から連続した0とシングルクオテーションを減算
        bool is_continue_zero = true;
        for (auto ch : num_str)
            if ((is_continue_zero = (is_continue_zero && ch == '0')) || ch == '\'')
                cnt--;
        return cnt;
    }

    // リテラルの接頭詞や、先頭の0、区切り文字のシングルクオテーションマーク以外をカウント
    // @tparam CharT 文字型
    // @param num_str 対象の数値文字列
    template <class CharT>
    inline constexpr std::size_t count_number_digits(const CharT* num_str)
    { return count_number_digits(std::basic_string_view<CharT>{num_str}); }

    // 入力文字列が正しいか検証
    // @tparam CharT 文字型
    // @tparam Traits 文字特性
    // @param num_str 対象の数値文字列
    // @param base_number 進数
    template <class CharT, class Traits = std::char_traits<CharT>>
    inline constexpr bool validate_input_number_string(
        std::basic_string_view<CharT, Traits> num_str,
        std::size_t base_number
    ) {
        using str_view_t = std::basic_string_view<CharT, Traits>;
        // 接頭詞を除外した部分の検証
        constexpr auto valid_without_prefix = [](str_view_t _num_str, std::size_t _base_number) -> bool {
            constexpr auto double_s_quote = TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, Traits, "''");
            constexpr auto npos = str_view_t::npos;

            // シングルクオテーションから開始する文字列はNG
            if (_num_str.starts_with(double_s_quote[0]))
                return false;
            // シングルクオテーションが連続して2回以上出現したらNG
            if (_num_str.find(double_s_quote) != npos)
                return false;
            // 想定外の文字が入力されていないか検査
            for (CharT ch : _num_str)
                if (ch != double_s_quote[0])
                    if (auto num = convert_char_to_num(ch); num < 0 || _base_number <= num)
                        return false;
            return true;
        };

        if (const auto prefix_len = get_literal_prefix_length(base_number); !prefix_len)
            return valid_without_prefix(num_str, base_number);
        else
            // 接頭詞検査のち除去部分の検査
            for (auto pref : get_literal_prefixes<CharT, Traits>(base_number))
                if (num_str.starts_with(pref))
                    return valid_without_prefix(num_str.substr(prefix_len), base_number);
        return false;
    }

    // 入力文字列が正しいか検証
    // @tparam CharT 文字型
    // @param num_str 対象の数値文字列
    // @param base_number 進数
    template <class CharT>
    inline constexpr bool validate_input_number_string(const CharT* num_str, std::size_t base_number)
    { return validate_input_number_string(std::basic_string_view<CharT>{num_str}, base_number); }

    // シングルクオテーションを除去、文字列の並び反転の上、数値配列に変換
    // @tparam ArrSize 結果配列サイズ
    // @tparam CharT 文字型
    // @tparam Traits 文字特性
    // @param num_str 対象の数値文字列
    // @param base_number 進数
    template <
        std::size_t ArrSize,
        class CharT,
        class Traits = std::char_traits<CharT>
    >
    inline constexpr std::array<int, ArrSize + 1> convert_str_to_number_array(
        std::basic_string_view<CharT, Traits> num_str,
        std::size_t base_number
    ) {
        if (!validate_input_number_string<CharT, Traits>(num_str, base_number))
            throw std::invalid_argument("Specified not number string.");

        num_str = num_str.substr(get_literal_prefix_length(base_number));
        constexpr CharT s_quote = TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, Traits, "'")[0];
        std::array<int, ArrSize + 1> number_array = {};

        // ranges の reverse_view による反転は clang で怒られたため、
        // インデックス操作で読み取り順序を反転させる
        for (int i = 0, ch_i = num_str.length() - 1; ch_i >= 0 && i <= ArrSize; ch_i--)
            if (const auto ch = num_str[ch_i]; ch != s_quote)
                number_array[i++] = convert_char_to_num(ch);
        return number_array;
    }

    // シングルクオテーションを除去、文字列の並び反転の上、数値配列に変換
    // @tparam ArrSize 結果配列サイズ
    // @tparam CharT 文字型
    // @param num_str 対象の数値文字列
    // @param base_number 進数
    template <std::size_t ArrSize, class CharT>
    inline constexpr auto convert_str_to_number_array(const CharT* num_str, std::size_t base_number)
    { return convert_str_to_number_array<ArrSize>(std::basic_string_view<CharT>{num_str}, base_number); }
}

#endif
