#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_LITERALS_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_LITERALS_HPP

#include TUNUM_COMMON_INCLUDE(fmpint/core.hpp)

namespace tunum::literals
{
    // -------------------------------------------
    // リテラル定義
    // -------------------------------------------

    namespace impl
    {
        // リテラルの接頭詞や、先頭の0、区切り文字のシングルクオテーションマーク以外をカウント
        template <char... IntegralLiteral>
        inline constexpr std::size_t count_number_digits()
        {
            constexpr char s[] = { IntegralLiteral..., '\0' };
            constexpr auto prefixes = std::array{
                std::string_view{"0b"},
                std::string_view{"0B"},
                std::string_view{"0x"},
                std::string_view{"0X"},
                std::string_view{"0"}
            };

            auto view = std::string_view{s};
            std::size_t cnt = view.length();
            for (auto prefix : prefixes)
                if (view.starts_with(prefix)) {
                    cnt -= prefix.length();
                    view.remove_prefix(prefix.length());
                    break;
                }

            bool is_continue_zero = true;
            for (auto ch : view)
                if ((is_continue_zero = is_continue_zero && ch == '0') || ch == '\'')
                    cnt--;
            return cnt;
        }

        // リテラルの接頭詞から進数を取得
        template <char... IntegralLiteral>
        inline constexpr std::size_t get_base_number()
        {
            constexpr char s[] = { IntegralLiteral..., '\0' };
            const auto view = std::string_view{s};
            if (view.starts_with("0b") || view.starts_with("0B"))
                return 2;
            if (view.starts_with("0x") || view.starts_with("0X"))
                return 16;
            if (view.starts_with("0"))
                return 8;
            return 10;
        }

        template <bool Signed, char... IntegralLiteral>
        inline constexpr auto make_fmpint()
        {
            using min_fmpint = fmpint<8, false>;
            constexpr std::size_t count_num = count_number_digits<IntegralLiteral...>();
            constexpr std::size_t base_number = get_base_number<IntegralLiteral...>();

            constexpr auto byte_size = ::tunum::alignment(
                ::tunum::calc_integral_storable_byte<base_number>(count_num) * 8,
                min_fmpint::base_data_digits2
            ) * min_fmpint::base_data_digits2 / 8;
            constexpr char s[] = { IntegralLiteral..., '\0' };

            using fmpint_t = fmpint<byte_size, Signed>;
            static_assert(
                fmpint_t::template _valid_input_number_string<base_number>(s),
                "Invalid integer literal."
            );
            return fmpint_t{s};
        }
    }

    // 符号ありリテラル
    template <char... IntegralLiteral>
    inline constexpr auto operator"" _fmp() { return impl::make_fmpint<true, IntegralLiteral...>(); }

    // 符号なしリテラル
    template <char... IntegralLiteral>
    inline constexpr auto operator"" _ufmp() { return impl::make_fmpint<false, IntegralLiteral...>(); }
}

#endif
