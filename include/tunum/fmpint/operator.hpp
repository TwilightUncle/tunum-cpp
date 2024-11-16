#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_OPERATOR_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_OPERATOR_HPP

#include TUNUM_COMMON_INCLUDE(fmpint/core.hpp)

namespace tunum
{
    // -------------------------------------------
    // 演算子オーバーロード(グローバル)
    // -------------------------------------------

#ifndef TUNUM_FUNC_MAKE_FMPINT_OPERATOR
#define TUNUM_FUNC_MAKE_FMPINT_OPERATOR(op_name, op) template <TuFmpIntegral T1, TuIntegral T2> \
    constexpr auto op_name(const T1& l, const T2& r) { return arithmetc_operation_result_t<T1, T2>{l} op r; } \
    template <std::integral T1, TuFmpIntegral T2> \
    constexpr auto op_name(T1 l, const T2&  r) { return arithmetc_operation_result_t<T1, T2>{l} op r; }
#endif

    template <TuFmpIntegral T>
    constexpr auto operator<<(const T& l, std::size_t r) { return T{l} <<= r; }

    template <TuFmpIntegral T>
    constexpr auto operator>>(const T& l, std::size_t r) { return T{l} >>= r; }

    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator|, |=)
    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator&, &=)
    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator^, ^=)

    template <TuFmpIntegral T1, TuIntegral T2>
    constexpr auto operator<=>(const T1& l, const T2& r)
    {
        using large_integral_t1 = get_large_integral_t<T1, T2, T1>;
        using large_integral_t2 = get_large_integral_t<T1, T2, T2>;
        return large_integral_t1{l}._compare(large_integral_t2{r});
    }
    template <TuFmpIntegral T>
    constexpr auto operator<=>(std::integral auto l, const T& r) { return T{l}._compare(r); }

    constexpr bool operator==(const TuFmpIntegral auto& l, const TuIntegral auto& r) { return (l <=> r) == 0; }
    constexpr bool operator==(std::integral auto l, const TuFmpIntegral auto& r) { return (l <=> r) == 0; }

    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator+, +=)
    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator-, -=)
    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator*, *=)
    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator/, /=)
    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator%, %=)

#undef TUNUM_FUNC_MAKE_FMPINT_OPERATOR
}

#endif
