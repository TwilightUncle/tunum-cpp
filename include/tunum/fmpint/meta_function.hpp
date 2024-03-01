#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_META_FUNCTION_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_META_FUNCTION_HPP

#include <concepts>

namespace tunum
{
    // 前方宣言
    template <std::size_t Bytes>
    struct fmpint;

    // -------------------------------------------
    // メタ関数・コンセプト
    // -------------------------------------------

    // fmpint かどうか判定
    template <class T>
    struct is_fmpint : public std::false_type {};
    template <std::size_t Bytes>
    struct is_fmpint<fmpint<Bytes>> : public std::true_type {};

    // fmpint かどうか判定
    template <class T>
    constexpr bool is_fmpint_v = is_fmpint<T>::value;

    // fmpint かどうか判定
    template <class T>
    concept TuFmpIntegral = is_fmpint_v<T>;

    // fmpint または 組み込み整数型 かどうか判定
    template <class T>
    concept TuIntegral = is_fmpint_v<T> || std::is_integral_v<T>;

    // 制約 TuIntegral が真となる型の byte サイズを取得
    template <class T>
    struct get_integral_size : public std::integral_constant<int, -1> {};
    template <std::integral T>
    struct get_integral_size<T> : public std::integral_constant<int, sizeof(T)> {};
    template <std::size_t Bytes>
    struct get_integral_size<fmpint<Bytes>> : public std::integral_constant<int, fmpint<Bytes>::size> {};

    // 制約 TuIntegral が真となる型の byte サイズを取得
    template <class T>
    constexpr int get_integral_size_v = get_integral_size<T>::value;

    // 制約 TuIntegral が真となる二つの型のうち、大きいほうと同じサイズの fmpint を返却
    template <TuIntegral T1, TuIntegral T2>
    struct get_large_integral : public std::type_identity<
        fmpint<(std::max)(
            get_integral_size_v<T1>,
            get_integral_size_v<T2>
        )>
    > {};

    // 制約 TuIntegral が真となる二つの型のうち、大きいほうと同じサイズの fmpint を返却
    template <TuIntegral T1, TuIntegral T2>
    using get_large_integral_t = typename get_large_integral<T1, T2>::type;
}

#endif
