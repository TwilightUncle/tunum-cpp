#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_LITERALS_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_LITERALS_HPP

#include TUNUM_COMMON_INCLUDE(fmpint/core.hpp)
#include TUNUM_COMMON_INCLUDE(utility.hpp)

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
            constexpr auto byte_size = ::tunum::alignment(
                ::tunum::calc_integral_storable_byte(sizeof...(IntegralLiteral)) * 8,
                min_fmpint::base_data_digits2
            ) * min_fmpint::base_data_digits2 / 8;
            constexpr char s[] = { IntegralLiteral..., '\0' };
            return fmpint<byte_size, Signed>{s};
        }
    }

    // 符号あり 10 進数リテラル
    template <char... IntegralLiteral>
    inline constexpr auto operator"" _fmp() { return impl::make_fmpint<true, IntegralLiteral...>(); }

    // // 符号あり 2 進数リテラル
    // template <char... IntegralLiteral>
    // inline constexpr auto operator"" _fmp2()
    // {
    //     constexpr auto byte_size = ::tunum::calc_integral_storable_byte(sizeof...(IntegralLiteral));
    //     using ret_t = fmpint<byte_size>;
    //     constexpr char s[] = { IntegralLiteral..., '\0' };
    //     return ::tunum::fmpint<byte_size>{s};
    // }
    
    // // 符号あり 8 進数リテラル
    // template <char... IntegralLiteral>
    // inline constexpr auto operator"" _fmp8()
    // {
    //     constexpr auto byte_size = ::tunum::calc_integral_storable_byte(sizeof...(IntegralLiteral));
    //     using ret_t = fmpint<byte_size>;
    //     constexpr char s[] = { IntegralLiteral..., '\0' };
    //     return ::tunum::fmpint<byte_size>{s};
    // }
    
    // // 符号あり 16 進数リテラル
    // template <char... IntegralLiteral>
    // inline constexpr auto operator"" _fmp16()
    // {
    //     constexpr auto byte_size = ::tunum::calc_integral_storable_byte(sizeof...(IntegralLiteral));
    //     using ret_t = fmpint<byte_size>;
    //     constexpr char s[] = { IntegralLiteral..., '\0' };
    //     return ::tunum::fmpint<byte_size>{s};
    // }

    // // 符号なし 10 進数リテラル
    // template <char... IntegralLiteral>
    // inline constexpr auto operator"" _ufmp()
    // {
    //     constexpr auto byte_size = ::tunum::calc_integral_storable_byte(sizeof...(IntegralLiteral));
    //     using ret_t = fmpint<byte_size>;
    //     constexpr char s[] = { IntegralLiteral..., '\0' };
    //     return ::tunum::fmpint<byte_size>{s};
    // }
    
    // // 符号なし 2 進数リテラル
    // template <char... IntegralLiteral>
    // inline constexpr auto operator"" _ufmp2()
    // {
    //     constexpr auto byte_size = ::tunum::calc_integral_storable_byte(sizeof...(IntegralLiteral));
    //     using ret_t = fmpint<byte_size>;
    //     constexpr char s[] = { IntegralLiteral..., '\0' };
    //     return ::tunum::fmpint<byte_size>{s};
    // }
    
    // // 符号なし 8 進数リテラル
    // template <char... IntegralLiteral>
    // inline constexpr auto operator"" _ufmp8()
    // {
    //     constexpr auto byte_size = ::tunum::calc_integral_storable_byte(sizeof...(IntegralLiteral));
    //     using ret_t = fmpint<byte_size>;
    //     constexpr char s[] = { IntegralLiteral..., '\0' };
    //     return ::tunum::fmpint<byte_size>{s};
    // }
    
    // // 符号なし 16 進数リテラル
    // template <char... IntegralLiteral>
    // inline constexpr auto operator"" _ufmp16()
    // {
    //     constexpr auto byte_size = ::tunum::calc_integral_storable_byte(sizeof...(IntegralLiteral));
    //     using ret_t = fmpint<byte_size>;
    //     constexpr char s[] = { IntegralLiteral..., '\0' };
    //     return ::tunum::fmpint<byte_size>{s};
    // }
}

#endif
