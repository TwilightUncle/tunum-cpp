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
        template <bool Signed, char... IntegralLiteral>
        inline constexpr auto make_fmpint()
        {
            using min_fmpint = fmpint<8, false>;
            constexpr char s[] = { IntegralLiteral..., '\0' };
            constexpr std::size_t count_num = ::tunum::count_number_digits(s);
            constexpr std::size_t base_number = ::tunum::get_base_number(s);
            static_assert(
                ::tunum::validate_input_number_string(s, base_number),
                "Invalid integer literal."
            );

            constexpr auto num_arr = ::tunum::convert_str_to_number_array<count_num>(s, base_number);
            constexpr auto byte_size = ::tunum::alignment(
                ::tunum::calc_base_number_digits(base_number, 2, num_arr),
                min_fmpint::base_data_digits2
            ) * min_fmpint::base_data_digits2 / 8;
            static_assert(byte_size);

            using fmpint_t = fmpint<byte_size, Signed>;
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
