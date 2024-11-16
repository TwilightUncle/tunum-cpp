#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MP_GET_INT_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MP_GET_INT_HPP

#include <bit>
#include TUNUM_COMMON_INCLUDE(submodule_loader.hpp)

namespace tunum
{
    // 前方宣言
    template <std::size_t Bytes, bool Signed>
    struct fmpint;
}

namespace tunum::tpfn
{
    // 任意サイズの符号あり整数型を取得
    // 2の累乗以外のByteSizeは直上の2累乗値に引き上げられる
    // @tparam ByteSize
    // @tparam Signed 取得する型の符号有無。デフォルトは符号あり
    template <std::size_t ByteSize, bool Signed = true>
    struct get_int : public get_int<std::bit_ceil(ByteSize), Signed> {};
    template <bool Signed> struct get_int<(1 << 0), Signed> : public std::conditional<Signed, std::int8_t, std::uint8_t> {};
    template <bool Signed> struct get_int<(1 << 1), Signed> : public std::conditional<Signed, std::int16_t, std::uint16_t> {};
    template <bool Signed> struct get_int<(1 << 2), Signed> : public std::conditional<Signed, std::int32_t, std::uint32_t> {};
    template <bool Signed> struct get_int<(1 << 3), Signed> : public std::conditional<Signed, std::int64_t, std::uint64_t> {};

    // 組み込みの整数型で表現できない場合、fmpintを返却
    template <std::size_t ByteSize, bool Signed>
    requires ((1 << 3) < ByteSize)
    struct get_int<ByteSize, Signed> : public std::type_identity<
        fmpint<std::bit_ceil(ByteSize), Signed>
    > {};

    // 任意サイズの符号なし整数型を取得
    // 2の累乗以外のByteSizeは直上の2累乗値に引き上げられる
    // @tparam ByteSize
    template <std::size_t ByteSize>
    struct get_uint : public get_int<ByteSize, false> {};
}

namespace tunum
{
    // 任意サイズの符号あり整数型を取得
    // 2の累乗以外のByteSizeは直上の2累乗値に引き上げられる
    // @tparam ByteSize
    // @tparam Signed 取得する型の符号有無。デフォルトは符号あり
    template <std::size_t ByteSize, bool Signed = true>
    using get_int_t = typename tpfn::get_int<ByteSize, Signed>::type;

    // 任意サイズの符号なし整数型を取得
    // 2の累乗以外のByteSizeは直上の2累乗値に引き上げられる
    // @tparam ByteSize
    template <std::size_t ByteSize>
    using get_uint_t = typename tpfn::get_uint<ByteSize>::type;
}

#endif
