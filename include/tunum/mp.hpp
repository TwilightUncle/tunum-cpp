// ---------------------------------------
// メタプログラミング
// ---------------------------------------
#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MP_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MP_HPP

#ifndef TUNUM_COMMON_INCLUDE
#define TUNUM_COMMON_INCLUDE(path) <tunum/path>
#endif

#include <concepts>
#include <bit>

namespace tunum
{
    // 前方宣言
    template <std::size_t Bytes, bool Signed>
    struct fmpint;

    // fmpint かどうか判定
    template <class T>
    struct is_fmpint : public std::false_type {};
    template <std::size_t Bytes, bool Signed>
    struct is_fmpint<fmpint<Bytes, Signed>> : public std::true_type {};

    // fmpint かどうか判定
    template <class T>
    constexpr bool is_fmpint_v = is_fmpint<T>::value;

    // 符号なし fmpint か判定
    template <class T>
    struct is_unsigned_fmpint : public std::false_type {};
    template <std::size_t Bytes>
    struct is_unsigned_fmpint<fmpint<Bytes, false>> : public std::true_type {};

    // 符号なし fmpint か判定
    template <class T>
    constexpr bool is_unsigned_fmpint_v = is_unsigned_fmpint<T>::value;

    // 制約 TuIntegral が真となる型の byte サイズを取得
    template <class T>
    struct get_integral_size : public std::integral_constant<int, -1> {};
    template <std::integral T>
    struct get_integral_size<T> : public std::integral_constant<int, sizeof(T)> {};
    template <std::size_t Bytes, bool Signed>
    struct get_integral_size<fmpint<Bytes, Signed>> : public std::integral_constant<int, fmpint<Bytes, Signed>::size> {};

    // 制約 TuIntegral が真となる型の byte サイズを取得
    template <class T>
    constexpr int get_integral_size_v = get_integral_size<T>::value;

    // 制約 TuIntegral が真となる二つの型のうち、大きいほうと同じサイズの fmpint を返却
    template <class T1, class T2, class T1_or_T2>
    requires ((is_fmpint_v<T1> || std::is_integral_v<T1>)
        && (is_fmpint_v<T2> || std::is_integral_v<T2>)
        && (is_fmpint_v<T1_or_T2> || std::is_integral_v<T1_or_T2>)
    )
    struct get_large_integral : public std::type_identity<
        fmpint<(std::max)(sizeof(T1), sizeof(T2)), !(is_unsigned_fmpint_v<T1_or_T2> || std::is_unsigned_v<T1_or_T2>)>
    > {};

    // 制約 TuIntegral が真となる二つの型のうち、大きいほうと同じサイズの fmpint を返却
    template <class T1, class T2, class T1_or_T2>
    using get_large_integral_t = typename get_large_integral<T1, T2, T1_or_T2>::type;

    // 任意サイズの符号あり整数型を取得
    // 2の累乗以外のByteSizeは直上の2累乗値に引き上げられる
    template <std::size_t ByteSize>
    struct get_int : public get_int<std::bit_ceil(ByteSize)> {};
    template <> struct get_int<(1 << 0)> : public std::type_identity<std::int8_t> {};
    template <> struct get_int<(1 << 1)> : public std::type_identity<std::int16_t> {};
    template <> struct get_int<(1 << 2)> : public std::type_identity<std::int32_t> {};
    template <> struct get_int<(1 << 3)> : public std::type_identity<std::int64_t> {};
    template <std::size_t ByteSize>
    requires ((1 << 3) < ByteSize)
    struct get_int<ByteSize> : public std::type_identity<
        fmpint<std::bit_ceil(ByteSize), true>
    > {};

    // 任意サイズの符号あり整数型を取得
    // 2の累乗以外のByteSizeは直上の2累乗値に引き上げられる
    template <std::size_t ByteSize>
    using get_int_t = typename get_int<ByteSize>::type;

    // 任意サイズの符号なし整数型を取得
    // 2の累乗以外のByteSizeは直上の2累乗値に引き上げられる
    template <std::size_t ByteSize>
    struct get_uint : public get_uint<std::bit_ceil(ByteSize)> {};
    template <> struct get_uint<(1 << 0)> : public std::type_identity<std::uint8_t> {};
    template <> struct get_uint<(1 << 1)> : public std::type_identity<std::uint16_t> {};
    template <> struct get_uint<(1 << 2)> : public std::type_identity<std::uint32_t> {};
    template <> struct get_uint<(1 << 3)> : public std::type_identity<std::uint64_t> {};
    template <std::size_t ByteSize>
    requires ((1 << 3) < ByteSize)
    struct get_uint<ByteSize> : public std::type_identity<
        fmpint<std::bit_ceil(ByteSize), false>
    > {};

    // 任意サイズの符号なし整数型を取得
    // 2の累乗以外のByteSizeは直上の2累乗値に引き上げられる
    template <std::size_t ByteSize>
    using get_uint_t = typename get_uint<ByteSize>::type;
}

#endif
